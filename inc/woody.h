/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   woody.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sgardner <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/22 22:48:07 by sgardner          #+#    #+#             */
/*   Updated: 2019/06/25 00:11:32 by sgardner         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WOODY_H
# define WOODY_H
# include "libft.h"
# include <elf.h>
# include <unistd.h>

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
	uint64_t	*key;
	void		*data;
	Elf64_Phdr	*code;
	Elf64_Shdr	*text;
	ssize_t		filesize;
	int			decrypt;
}				t_woody;

#define E_FNOTENC	"binary is not encrypted with woody"
#define E_NOSPACE	"could not find space for decryptor"
#define E_ALREADY	"binary has already been encrypted"
#define E_INVALID   "invalid file, must be linux elf64"
#define E_READ		"failed to read input file"
#define E_WRITE		"could not write output file"
#define E_BADKEY	"invalid key, must be 16 characters long (0-9, a-f)"
#define E_USAGE		"usage: ./woody_woodpacker [-d -k key] binary"
#define MSG_ENC		"encryption key = %#.16llx\n"
#define MSG_DEC		"decryption key = %#.16llx\n"
#define F_DEC		"unwoody"
#define F_ENC		"woody"
#define F_PRNG		"/dev/urandom"
#define BASE16	    "0123456789abcdef"

/*
** decrypt.c
*/

int				is_encrypted(t_woody *woody);
int				decrypt_binary(t_woody *woody);

/*
** encrypt.c
*/

int				create_encrypted_binary(t_woody *woody);
uint64_t		encrypt_code(t_woody *woody);

/*
** load.c
*/

int				is_valid_elf64(t_woody *woody);
int				load_file(t_woody *woody, char *f);

/*
** main.c
*/

int				fatal_error(void *p, char *s);

/*
** segment.c
*/

size_t			get_cavity_size(t_woody *woody);
Elf64_Phdr		*get_code_segment(void *data);
Elf64_Shdr		*get_text_section(void *data);

extern unsigned char	g_decryptor[];
extern unsigned int		g_decryptor_len;
#endif
