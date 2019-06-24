/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   woody.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sgardner <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/22 22:48:07 by sgardner          #+#    #+#             */
/*   Updated: 2019/06/23 18:48:21 by sgardner         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WOODY_H
# define WOODY_H
# include "libft.h"
# include <elf.h>

typedef struct	s_decryptor_values
{
	uint64_t	entry;
	uint64_t	address;
	uint64_t	size;
	uint64_t	key;
	uint64_t	pos;
}				t_dv;

typedef struct	s_woody
{
	void		*data;
	Elf64_Phdr	*code;
	Elf64_Shdr	*text;
	ssize_t		filesize;
}				t_woody;

/*
** encrypt.c
*/

void			create_encrypted_binary(t_woody *woody);

/*
** load.c
*/

int				is_valid_elf64(t_woody *woody);
int				load_file(t_woody *woody, char *f);

/*
** segment.c
*/

size_t			get_cavity_size(t_woody *woody);
Elf64_Phdr		*get_code_segment(void *data);
Elf64_Shdr		*get_text_section(void *data);

extern unsigned char	g_decryptor[];
extern unsigned int		g_decryptor_len;
#endif
