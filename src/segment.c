/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   segment.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asarandi <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/23 18:37:04 by asarandi          #+#    #+#             */
/*   Updated: 2019/06/23 18:39:03 by sgardner         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody.h"

static int	has_overlap(void *data, Elf64_Phdr *target, size_t idx)
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

size_t		get_cavity_size(t_woody *woody)
{
	void	*start;
	void	*pos;

	start = woody->data + woody->code->p_offset + woody->code->p_filesz;
	pos = start;
	while (pos < woody->data + woody->filesize
		&& !*(unsigned char *)pos
		&& !has_overlap(woody->data, woody->code, pos - woody->data))
		++pos;
	return (pos - start);
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
