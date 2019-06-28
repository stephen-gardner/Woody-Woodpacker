/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   decrypt.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sgardner <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/24 22:31:18 by sgardner          #+#    #+#             */
/*   Updated: 2019/06/28 00:45:45 by stephen          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody.h"
#include <stdio.h>

int		is_encrypted(t_woody *woody)
{
	void		*data;
	size_t		size;
	size_t		original_size;

	size = g_decryptor_len - sizeof(t_dv);
	original_size = woody->code->p_filesz - g_decryptor_len;
	data = woody->data + woody->code->p_offset + original_size;
	return (!ft_memcmp(data, g_decryptor, size));
}

int		decrypt_binary(t_woody *woody)
{
	Elf64_Ehdr	*ehdr;
	t_dv		*dv;
	void		*ptr;

	if (!is_encrypted(woody))
		return (fatal_error(woody->data, E_FNOTENC));
	ehdr = woody->data;
	woody->code->p_memsz -= g_decryptor_len;
	woody->code->p_filesz -= g_decryptor_len;
	woody->code->p_flags ^= PF_W;
	ptr = woody->data + woody->code->p_offset + woody->code->p_filesz;
	dv = ptr + (g_decryptor_len - sizeof(t_dv));
	ehdr->e_entry = dv->entry;
	woody->key = &dv->key;
	printf(MSG_DEC, *woody->key);
	encrypt_code(woody);
	ft_memset(ptr, 0, g_decryptor_len);
	return (0);
}
