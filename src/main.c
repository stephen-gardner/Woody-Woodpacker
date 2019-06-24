/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asarandi <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/14 23:42:52 by asarandi          #+#    #+#             */
/*   Updated: 2019/06/23 18:40:37 by sgardner         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody.h"
#include "libftprintf.h"

static int	fatal_error(void *p, char *s)
{
	free(p);
	ft_printf("%s\n", s);
	return (0);
}

int			main(int ac, char **av)
{
	t_woody		woody;

	ft_memset(&woody, 0, sizeof(woody));
	if (ac != 2)
		return (fatal_error(0, "input file name missing"));
	if (!load_file(&woody, av[1]))
		return (fatal_error(0, "failed to read input file"));
	if (!is_valid_elf64(&woody))
		return (fatal_error(woody.data, "invalid file"));
	if (get_cavity_size(&woody) < g_decryptor_len)
		return (fatal_error(woody.data, "could not find space for decryptor"));
	create_encrypted_binary(&woody);
	free(woody.data);
	return (0);
}
