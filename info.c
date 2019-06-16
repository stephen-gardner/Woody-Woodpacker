#include <elf.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

char *file_get_contents(char *f)
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
    if (ret == st.st_size)  /* maybe include a check for minimum valid file size ? */
        return data;
    if (data)
        free(data);
    return NULL;
}

int quit_msg(char *s)
{
    printf("%s\n", s);
    return 0;
}

void print_section64_header(void *data)
{
    Elf64_Ehdr  *ehdr = data;
    Elf64_Shdr  *shdr = data + ehdr->e_shoff;
    printf("[ section header ]\n");
    for (int i = 0; i < ehdr->e_shnum; i++)
    {
        printf("sh_name     = %x \n", shdr->sh_name);
        printf("sh_type     = %x \n", shdr->sh_type);       
        printf("sh_flags    = %lx\n", shdr->sh_flags);      
        printf("sh_addr     = %lx\n", shdr->sh_addr);       
        printf("sh_offset   = %lx\n", shdr->sh_offset);     
        printf("sh_size     = %lx\n", shdr->sh_size);       
        printf("sh_link     = %x \n", shdr->sh_link);       
        printf("sh_info     = %x \n", shdr->sh_info);       
        printf("sh_addralign= %lx\n", shdr->sh_addralign);
        printf("sh_entsize  = %lx\n", shdr->sh_entsize);
        printf("\n");
        shdr++;
    }
}

void print_program64_header(void *data)
{
    Elf64_Ehdr  *ehdr = data;
    Elf64_Phdr  *phdr = data + ehdr->e_phoff;
    printf("[ program header ]\n");
    for (int i = 0; i < ehdr->e_phnum; i++)
    {
        printf("   p_type   = %x \n", phdr->p_type  ); 
        printf("   p_flags  = %x \n", phdr->p_flags );
        printf("   p_offset = %lx\n", phdr->p_offset);
        printf("   p_vaddr  = %lx\n", phdr->p_vaddr );
        printf("   p_paddr  = %lx\n", phdr->p_paddr );
        printf("   p_filesz = %lx\n", phdr->p_filesz);
        printf("   p_memsz  = %lx\n", phdr->p_memsz );
        printf("   p_align  = %lx\n", phdr->p_align );
        printf("\n");
        phdr++;
    }
}

void print_elf64_header(void *data)
{
    Elf64_Ehdr  *ehdr = data;

    printf("[   elf64 header (0x%lx bytes)]\n", sizeof(Elf64_Ehdr));
    printf("    e_ident = ");
    for (int i = 0; i < EI_NIDENT; i++)
        printf("%02x ", ehdr->e_ident[i]);
    printf("\n     e_type = %x\n", ehdr->e_type);
    printf("  e_machine = %x\n", ehdr->e_machine);
    printf("  e_version = %x\n", ehdr->e_version);
    printf("    e_entry = %lx\n", ehdr->e_entry);
    printf("    e_phoff = %lx\n", ehdr->e_phoff);
    printf("    e_shoff = %lx\n", ehdr->e_shoff);
    printf("    e_flags = %x\n", ehdr->e_flags);
    printf("   e_ehsize = %x\n", ehdr->e_ehsize);
    printf("e_phentsize = %x\n", ehdr->e_phentsize);
    printf("    e_phnum = %x\n", ehdr->e_phnum);
    printf("e_shentsize = %x\n", ehdr->e_shentsize);
    printf("    e_shnum = %x\n", ehdr->e_shnum);
    printf(" e_shstrndx = %x\n", ehdr->e_shstrndx);
    printf("\n");
}



int is_valid_elf64(void *data)
{
    Elf64_Ehdr  *ehdr = data;
   
    const unsigned char elf64_valid_header[] = {
      0x7f, 0x45, 0x4c, 0x46, 0x02, 0x01, 0x01, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x02, 0x00, 0x3e, 0x00, 0x01, 0x00, 0x00, 0x00
    };

    if (memcmp(data, elf64_valid_header, sizeof(elf64_valid_header)))
        return 0;
    if (ehdr->e_phentsize != sizeof(Elf64_Phdr)) /* XXX wtf */
        return 0;
    if (ehdr->e_shentsize != sizeof(Elf64_Shdr)) /* XXX wtf */
        return 0;
    print_elf64_header(data);
    print_program64_header(data);    
    print_section64_header(data);
    return 1;
}

int main(int ac, char **av)
{
    char    *data;

    if (ac != 2)
        return quit_msg("input file name missing");
    if (!(data = file_get_contents(av[1])))
        return quit_msg("failed to read input file");
    if (is_valid_elf64(data))
        printf("file ok\n");
    else
        printf("file ko\n");
    free(data);
    return 0;
}
