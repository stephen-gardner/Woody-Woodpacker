/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asarandi <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/14 23:42:52 by asarandi          #+#    #+#             */
/*   Updated: 2019/06/23 00:15:09 by sgardner         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "elf.h"
#include "libft.h"
#include "libftprintf.h"
#include "woody.h"
#include <fcntl.h>

off_t	get_file_size(int fd)
{
	off_t	result;

	if ((result = lseek(fd, 0, SEEK_END)) == -1
		|| lseek(fd, 0, SEEK_SET) == -1)
		return (-1);
	return (result);
}

char	*file_get_contents(char *f, ssize_t *size)
{
	char	*data;
	ssize_t	ret;
	int		fd;

	data = NULL;
	if ((fd = open(f, O_RDONLY)) == -1
		|| (*size = get_file_size(fd)) == -1
		|| !(data = ft_memalloc(*size))
		|| (ret = read(fd, data, *size)) != *size)
	{
		close(fd);
		free(data);
		return (NULL);
	}
	close(fd);
	return (data);
}

uint64_t	get_random_key(void)
{
	int			fd;
	uint64_t	data;

	if ((fd = open("/dev/urandom", O_RDONLY)) == -1)
		return (0xDEADBEEFBADC0FFE);
	if (read(fd, &data, 8) != 8)
		return (0xFEEDD0D0CACAC0DE);
	close(fd);
	return (data);
}

int	file_put_contents(char *f, void *d, ssize_t s, int m)
{
	int		fd;
	ssize_t	ret;

	if ((fd = open(f, O_WRONLY | O_CREAT | O_TRUNC, m)) == -1)
		return (0);
	ret = write(fd, d, s);
	close(fd);
	return (ret == s);
}

Elf64_Phdr	*get_code_segment(void *data)
{
	Elf64_Ehdr	*ehdr;
	Elf64_Phdr	*phdr;
	int			i;

	ehdr = data;
	phdr = data + ehdr->e_phoff;
	i = -1;
	while (++i < ehdr->e_phnum)
	{
		if (phdr[i].p_vaddr == phdr[i].p_paddr
			&& phdr[i].p_memsz == phdr[i].p_filesz
			&& phdr[i].p_type == 1
			&& phdr[i].p_flags == 5
			&& ehdr->e_entry >= phdr[i].p_vaddr
			&& ehdr->e_entry < phdr[i].p_vaddr + phdr[i].p_memsz)
			return (&phdr[i]);
	}
	return (NULL);
}

int	is_overlap(void *data, Elf64_Phdr *target, size_t idx)
{
	Elf64_Ehdr	*ehdr;
	Elf64_Phdr	*phdr;
	int			i;

	ehdr = data;
	if (idx < sizeof(Elf64_Ehdr))
		return (1);
	if (idx >= ehdr->e_phoff
		&& idx < (ehdr->e_phoff + (sizeof(Elf64_Phdr) * ehdr->e_phnum)))
		return (1);
	i = -1;
	phdr = data + ehdr->e_phoff;
	while (++i < ehdr->e_phnum)
	{
		if (idx >= phdr[i].p_offset
			&& idx < phdr[i].p_offset + phdr[i].p_filesz
			&& &phdr[i] != target)
			return (1);
	}
	return (0);
}

size_t	get_cavity_size(void *data, Elf64_Phdr *phdr, ssize_t fsize)
{
	void	*start;
	void	*pos;

	start = data + phdr->p_offset + phdr->p_filesz;
	pos = start;
	while (pos < data + fsize
		&& !*(unsigned char *)pos
		&& !is_overlap(data, phdr, pos - data))
		++pos;
	return (pos - start);
}

Elf64_Shdr	*get_text_section(void *data)
{
	Elf64_Ehdr	*ehdr;
	Elf64_Shdr	*shdr;
	char		*strings;
	int			i;

	strings = NULL;
	ehdr = data;
	shdr = data + ehdr->e_shoff;
	if (!ehdr->e_shoff || !ehdr->e_shnum || ehdr->e_shstrndx >= ehdr->e_shnum)
		return (NULL);
	strings = (char *)data + shdr[ehdr->e_shstrndx].sh_offset;
	i = -1;
	while (++i < ehdr->e_shnum)
	{
		if (shdr[i].sh_type == SHT_PROGBITS
			&& (shdr[i].sh_flags & SHF_EXECINSTR)
			&& !ft_strcmp(".text", strings + shdr[i].sh_name))
			return (&shdr[i]);
	}
	return (NULL);
}

int	is_valid_header(void *data)
{
	static const unsigned char	elf64_sysv[] = {
		0x7f, 0x45, 0x4c, 0x46, 0x02, 0x01, 0x01, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	};
	static const unsigned char	elf64_gnu[] = {
		0x7f, 0x45, 0x4c, 0x46, 0x02, 0x01, 0x01, 0x03,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	};

	return (!ft_memcmp(data, elf64_sysv, sizeof(elf64_sysv))
		|| !ft_memcmp(data, elf64_gnu, sizeof(elf64_gnu)));
}

int	is_valid_elf64(void *data)
{
	Elf64_Ehdr	*ehdr;

	ehdr = data;
	return (is_valid_header(data)
		&& ehdr->e_phentsize == sizeof(Elf64_Phdr)
		&& ehdr->e_shentsize == sizeof(Elf64_Shdr)
		&& ((ehdr->e_type == ET_EXEC) || (ehdr->e_type == ET_DYN))
		&& ehdr->e_machine == EM_X86_64
		&& ehdr->e_version == EV_CURRENT
		&& get_code_segment(data)
		&& get_text_section(data));
}

int	free_msg_quit(void *p, char *s)
{
	free(p);
	ft_printf("%s\n", s);
	return (0);
}

uint64_t	elf64_encrypt(void *data, size_t size)
{
	uint64_t	key;
	uint64_t	key_copy;
	size_t		i;

	key = get_random_key();
	key_copy = key;
	i = -1;
	while (++i < size)
	{
		*(unsigned char *)(data + i) ^= key & 0xff;
		key = (key >> 8) | ((key & 0xff) << 56);
	}
	return (key_copy);
}

void	elf64_insert(void *data, Elf64_Phdr *phdr)
{
	Elf64_Ehdr	*ehdr;
	Elf64_Shdr	*text;
	t_dv		dv;
	void		*ptr;
	ssize_t		k;

	ehdr = data;
	text = get_text_section(data);
	dv.entry = ehdr->e_entry;
	ehdr->e_entry = phdr->p_vaddr + phdr->p_memsz;
	dv.pos = ehdr->e_type == ET_DYN ? ehdr->e_entry : 0;
	dv.size = text->sh_size;
	dv.address = text->sh_addr;
	dv.key = elf64_encrypt(data + text->sh_offset, text->sh_size);
	ft_printf("encryption key = 0x%lx\n", dv.key);
	ptr = data + phdr->p_offset + phdr->p_filesz;
	phdr->p_memsz += g_decryptor_len;
	phdr->p_filesz += g_decryptor_len;
	phdr->p_flags |= PF_X | PF_W | PF_R;
	k = g_decryptor_len - sizeof(t_dv);
	ft_memcpy(ptr, g_decryptor, k);
	ft_memcpy(ptr + k, &dv, sizeof(t_dv));
}

int	main(int ac, char **av)
{
	char		*data;
	Elf64_Phdr	*phdr;
	ssize_t		fsize;

	if (ac != 2)
		return (free_msg_quit(0, "input file name missing"));
	fsize = 0;
	if (!(data = file_get_contents(av[1], &fsize)))
		return (free_msg_quit(0, "failed to read input file"));
	if (!is_valid_elf64(data))
		return (free_msg_quit(data, "file not valid"));
	if (!(phdr = get_code_segment(data)))
		return (free_msg_quit(data, "could not find code segment"));
	if (get_cavity_size(data, phdr, fsize) < g_decryptor_len)
		return (free_msg_quit(data, "could not find suitable cavity"));
	elf64_insert(data, phdr);
	if (!file_put_contents("woody", data, fsize, 0755))
		ft_printf("could not write output file");
	free(data);
	return (0);
}
