#include <elf.h>
#include <time.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

extern unsigned char decryptor[];
extern unsigned int decryptor_len;

char *file_get_contents(char *f, ssize_t *size)
{
    int         fd;
    ssize_t     ret;
    struct stat st;
    char        *data;

    if ((fd = open(f, O_RDONLY)) == -1)
        return NULL;
    data = NULL;
    if (fstat(fd, &st) == 0)
        data = malloc(st.st_size);
    ret = -1;
    if (data)
        ret = read(fd, data, st.st_size);
    close(fd);
    *size = st.st_size;
    if (ret == st.st_size)  /* maybe include a check for minimum valid file size ? */
        return data;
    if (data)
        free(data);
    return NULL;
}

int file_put_contents(char *f, void *d, ssize_t s, int m)
{
    int     fd;
    ssize_t ret;

    fd = open(f, O_WRONLY | O_CREAT | O_TRUNC, m);
    if (fd == -1)
        return 0;
    ret = write(fd, d, s);
    close(fd);
    return ret == s ? 1:0;
}

Elf64_Phdr  *get_code_segment(void *data)
{
    Elf64_Ehdr  *ehdr = data;
    Elf64_Phdr  *phdr = data + ehdr->e_phoff;

    for (int i = 0; i < ehdr->e_phnum; i++)
    {
        if ((phdr->p_vaddr == phdr->p_paddr) &&
            (phdr->p_memsz == phdr->p_filesz) &&
            (phdr->p_type == 1) &&
            (phdr->p_flags == 5) &&
            (ehdr->e_entry >= phdr->p_vaddr) &&
            (ehdr->e_entry < phdr->p_vaddr + phdr->p_memsz))
            return phdr;
        phdr++;
    }
    return NULL;
}


int is_overlap(void *data, Elf64_Phdr *target, size_t idx)
{
    Elf64_Ehdr  *ehdr = data;
    Elf64_Phdr  *phdr = data + ehdr->e_phoff;

    if (idx < sizeof(Elf64_Ehdr))
        return 1;
    if ((idx >= ehdr->e_phoff) && (idx < (ehdr->e_phoff + (sizeof(Elf64_Phdr) * ehdr->e_phnum))))
        return 1;
    for (int i = 0; i < ehdr->e_phnum; i++)
    {
        if ((idx >= phdr->p_offset) && (idx < (phdr->p_offset + phdr->p_filesz))) {
            if (phdr != target)
                return 1;
        }
        phdr++;
    }
    return 0;
}

size_t get_cavity_size(void *data, Elf64_Phdr *phdr, ssize_t fsize)
{
    void    *ptr_start;
    void    *ptr;

    ptr_start = data + phdr->p_offset + phdr->p_filesz;
    ptr = ptr_start;
    while (ptr < (data + fsize))
    {
        if (*(unsigned char *)ptr)
            break ;
        if (is_overlap(data, phdr, ptr-data))
            break ;
        ptr++;
    }
    return ptr-ptr_start;
}

Elf64_Shdr	*get_text_section(void *data)
{

    Elf64_Ehdr  *ehdr = data;
    Elf64_Shdr  *shdr = data + ehdr->e_shoff;
    char        *strings = NULL;

    if ((!ehdr->e_shoff) || (!ehdr->e_shnum) || (ehdr->e_shstrndx >= ehdr->e_shnum))
       return NULL;
	strings = (char *)data + shdr[ehdr->e_shstrndx].sh_offset;
	for (int i=0; i < ehdr->e_shnum; i++) {
        if ((shdr[i].sh_type == SHT_PROGBITS) && (shdr[i].sh_flags & SHF_EXECINSTR)) {
            if (!strcmp(".text", strings + shdr[i].sh_name)) {
                return &shdr[i];
            }
        }
	}
    return NULL ;
}

int is_valid_header(void *data)
{
    const unsigned char elf64_sysv[] = {
      0x7f, 0x45, 0x4c, 0x46, 0x02, 0x01, 0x01, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    };

    const unsigned char elf64_gnu[] = {
      0x7f, 0x45, 0x4c, 0x46, 0x02, 0x01, 0x01, 0x03,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    };
    
    int result = 0;

    if (!memcmp(data, elf64_sysv, sizeof(elf64_sysv)))
        result = 1;
    if (!memcmp(data, elf64_gnu, sizeof(elf64_gnu)))
        result = 1;

    return result;
}

int is_valid_elf64(void *data)
{
    Elf64_Ehdr  *ehdr = data;

    if (!is_valid_header(data))
        return 0;
    if (ehdr->e_phentsize != sizeof(Elf64_Phdr)) /* XXX wtf */
        return 0;
    if (ehdr->e_shentsize != sizeof(Elf64_Shdr)) /* XXX wtf */
        return 0;
    if ((ehdr->e_type != ET_EXEC) && (ehdr->e_type != ET_DYN))  /* ET_EXEC == no pie */
        return 0;
    if ((ehdr->e_machine != EM_X86_64) || (ehdr->e_version != EV_CURRENT))
        return 0;
    if ((!get_code_segment(data)) || (!get_text_section(data)))
        return 0;
    return 1;
}

int free_msg_quit(void *p, char *s)
{
    if (p)
        free(p);
    printf("%s\n", s);
    return 0;
}

typedef struct      s_decryptor_values
{
    uint64_t        entry;
    uint64_t        address;
    uint64_t        size;
    uint64_t        key;
    uint64_t        pos;
}                   t_dv;

uint64_t elf64_encrypt(void *data, size_t size)  /* return 64 bit key */
{
    uint64_t    key, key_copy;
    struct      timespec tp;

    (void)clock_gettime(CLOCK_REALTIME, &tp);
    srand(tp.tv_sec ^ tp.tv_nsec);  /* try to figure out the key from file creation time */
    key = rand();
    key = (key << 32) | rand();
    key_copy = key;
    for (size_t i = 0; i < size; i++) {
        *(unsigned char *)(data + i) ^= key & 0xff;
        key = (key >> 8) | ((key & 0xff) << 56);
    }
    return key_copy;
}


void elf64_insert(void *data, Elf64_Phdr *phdr, ssize_t fsize)
{
    Elf64_Ehdr  *ehdr = data;
    Elf64_Shdr  *text = get_text_section(data);
    t_dv        dv;
    void        *ptr;
    ssize_t     k;

    (void)fsize;
    dv.entry = ehdr->e_entry;
    ehdr->e_entry = phdr->p_vaddr + phdr->p_memsz;
    dv.pos = ehdr->e_type == ET_DYN ? ehdr->e_entry : 0;

    dv.size = text->sh_size;
    dv.address = text->sh_addr;

    dv.key = elf64_encrypt(data + text->sh_offset, text->sh_size);

    ptr = data + phdr->p_offset + phdr->p_filesz;   //before modifying p_filesz

    phdr->p_memsz += decryptor_len;
    phdr->p_filesz += decryptor_len;
    phdr->p_flags |= PF_X | PF_W | PF_R;
    k = decryptor_len - sizeof(t_dv);
    memcpy(ptr, decryptor, k);
    memcpy(ptr + k, &dv, sizeof(t_dv));
}

int main(int ac, char **av)
{
    char        *data;
    Elf64_Phdr  *phdr;
    ssize_t     fsize;

    if (ac != 2)
        return free_msg_quit(0, "input file name missing");
    fsize = 0;
    if (!(data = file_get_contents(av[1], &fsize)))
        return free_msg_quit(0, "failed to read input file");
    if (!is_valid_elf64(data))
        return free_msg_quit(data, "file not valid");
    if (!(phdr = get_code_segment(data)))
        return free_msg_quit(data, "could not find code segment");
    if (get_cavity_size(data, phdr, fsize) < decryptor_len)
        return free_msg_quit(data, "could not find suitable cavity");
    elf64_insert(data, phdr, fsize);
    if (!file_put_contents("woody", data, fsize, 0755))
        printf("could not write output file");
    free(data);
    return 0;
}
