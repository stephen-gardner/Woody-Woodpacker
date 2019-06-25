/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   encrypt.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asarandi <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/23 00:56:32 by asarandi          #+#    #+#             */
/*   Updated: 2019/06/24 20:15:39 by sgardner         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody.h"
#include "ft_printf.h"
#include <fcntl.h>

static uint64_t	get_random_key(void)
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

static uint64_t	encrypt_code(void *data, size_t size)
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
		key = (key >> 1) | ((key & 0x01) << 63);
	}
	return (key_copy);
}

static int		write_output(t_woody *woody)
{
	int		fd;
	ssize_t	bytes;

	if ((fd = open("woody", O_WRONLY | O_CREAT | O_TRUNC, 0755)) == -1)
		return (0);
	bytes = write(fd, woody->data, woody->filesize);
	close(fd);
	return (bytes == woody->filesize);
}

void			create_encrypted_binary(t_woody *woody)
{
	Elf64_Ehdr	*ehdr;
	t_dv		dv;
	void		*ptr;
	ssize_t		k;

	ehdr = woody->data;
	dv.entry = ehdr->e_entry;
	ehdr->e_entry = woody->code->p_vaddr + woody->code->p_memsz;
	dv.pos = ehdr->e_type == ET_DYN ? ehdr->e_entry : 0;
	dv.size = woody->text->sh_size;
	dv.address = woody->text->sh_addr;
	dv.key = encrypt_code(woody->data + woody->text->sh_offset,
				woody->text->sh_size);
	ft_printf("encryption key = 0x%lx\n", dv.key);
	ptr = woody->data + woody->code->p_offset + woody->code->p_filesz;
	woody->code->p_memsz += g_decryptor_len;
	woody->code->p_filesz += g_decryptor_len;
	woody->code->p_flags |= PF_X | PF_W | PF_R;
	k = g_decryptor_len - sizeof(t_dv);
	ft_memcpy(ptr, g_decryptor, k);
	ft_memcpy(ptr + k, &dv, sizeof(t_dv));
	if (!write_output(woody))
		ft_printf("could not write output file");
}
