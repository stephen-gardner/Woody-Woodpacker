/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asarandi <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/14 23:42:52 by asarandi          #+#    #+#             */
/*   Updated: 2019/06/24 22:03:04 by sgardner         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody.h"
#include "ft_getopt.h"
#include "ft_printf.h"
#include <stdlib.h>

static int	fatal_error(void *p, char *s)
{
	free(p);
	ft_dprintf(2, "%s\n", s);
	return (1);
}

#define BASE	"0123456789abcdef"

static int	set_key(t_woody *woody, const char *arg)
{
	static uint64_t	key;
	char			*found;
	int				i;

	if (*arg && *arg == '0' && *(arg + 1) && TO_LOWER(*(arg + 1)) == 'x')
		arg += 2;
	i = -1;
	while (arg[++i])
	{
		if (i > 15 || !(found = ft_strchr(BASE, TO_LOWER(arg[i]))))
			return (0);
		key = (key * 16) + (found - BASE);
	}
	if (i == 16)
	{
		woody->key = &key;
		return (1);
	}
	return (0);
}

static int	parse_flags(t_woody *woody, int ac, char *const av[])
{
	char	f;

	while ((f = ft_getopt(ac, av, "k:")) != -1)
	{
		if (f == 'k' && !set_key(woody, g_optarg))
		{
			ft_dprintf(2, "Invalid key, must be 8-byte hex value\n");
			return (0);
		}
		else if (f == '?')
			return (0);
	}
	return (1);
}

int			main(int ac, char **av)
{
	t_woody		woody;

	ft_memset(&woody, 0, sizeof(woody));
	if (ac < 2)
		return (fatal_error(0, "Usage: ./woody_woodpacker [-k key] binary"));
	if (!parse_flags(&woody, ac, av))
		return (1);
	if (!load_file(&woody, av[g_optind]))
		return (fatal_error(0, "failed to read input file"));
	if (!is_valid_elf64(&woody))
		return (fatal_error(woody.data, "invalid file"));
	if (get_cavity_size(&woody) < g_decryptor_len)
		return (fatal_error(woody.data, "could not find space for decryptor"));
	create_encrypted_binary(&woody);
	free(woody.data);
	return (0);
}
