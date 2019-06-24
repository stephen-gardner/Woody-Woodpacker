/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   load.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asarandi <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/23 18:24:03 by asarandi          #+#    #+#             */
/*   Updated: 2019/06/23 20:38:43 by sgardner         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody.h"
#include <fcntl.h>
#include <stdlib.h>

static int		is_valid_header(void *data)
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

int				is_valid_elf64(t_woody *woody)
{
	Elf64_Ehdr	*ehdr;

	ehdr = woody->data;
	return (is_valid_header(woody->data)
		&& ehdr->e_phentsize == sizeof(Elf64_Phdr)
		&& ehdr->e_shentsize == sizeof(Elf64_Shdr)
		&& ((ehdr->e_type == ET_EXEC) || (ehdr->e_type == ET_DYN))
		&& ehdr->e_machine == EM_X86_64
		&& ehdr->e_version == EV_CURRENT
		&& (woody->code = get_code_segment(woody->data))
		&& (woody->text = get_text_section(woody->data)));
}

static off_t	get_filesize(int fd)
{
	off_t	result;

	if ((result = lseek(fd, 0, SEEK_END)) == -1
		|| lseek(fd, 0, SEEK_SET) == -1)
		return (-1);
	return (result);
}

int				load_file(t_woody *woody, char *f)
{
	int		fd;

	if ((fd = open(f, O_RDONLY)) == -1
		|| (woody->filesize = get_filesize(fd)) == -1
		|| !(woody->data = ft_memalloc(woody->filesize))
		|| read(fd, woody->data, woody->filesize) != woody->filesize)
	{
		close(fd);
		free(woody->data);
		return (0);
	}
	close(fd);
	return (1);
}
