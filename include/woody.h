/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   woody.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sgardner <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/22 22:48:07 by sgardner          #+#    #+#             */
/*   Updated: 2019/06/22 23:10:20 by sgardner         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WOODY_H
# define WOODY_H

typedef struct	s_decryptor_values
{
	uint64_t	entry;
	uint64_t	address;
	uint64_t	size;
	uint64_t	key;
	uint64_t	pos;
}				t_dv;

extern unsigned char	g_decryptor[];
extern unsigned int		g_decryptor_len;
#endif
