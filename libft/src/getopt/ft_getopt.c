/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_getopt.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sgardner <stephenbgardner@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/03 16:20:06 by sgardner          #+#    #+#             */
/*   Updated: 2019/06/28 01:20:09 by stephen          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_getopt.h"
#include "libft.h"
#include <stdio.h>
#include <unistd.h>

char		*g_optarg = NULL;
int			g_optind = 1;
int			g_opterr = 1;
int			g_optopt = '\0';
int			g_optreset = 0;

static int		opt_error(const char *pname, const char *optstring,
					t_bool no_arg)
{
	static const char	*msg[2] = {
		"illegal option",
		"option requires an argument"
	};

	if (g_opterr && *optstring != ':')
		dprintf(2, "%s: %s -- %c\n", pname, msg[no_arg], g_optopt);
	return ((no_arg && *optstring == ':') ? ':' : '?');
}

static t_bool	set_arg(int ac, char *const av[], char **pos, const char *op)
{
	t_bool	optional;

	optional = (*op && *(op + 2) == ':');
	if (**pos)
		g_optarg = *pos;
	else if (g_optind < ac && (!optional || *av[g_optind] != '-'))
		g_optarg = av[g_optind];
	if (!optional || g_optarg)
		++g_optind;
	*pos = NULL;
	return (g_optarg != NULL || optional);
}

static int		process_arg(int ac, char *const av[], const char *optstring)
{
	static char	*pos;
	const char	*op;

	if (!pos || !*pos || g_optreset == 1)
		pos = av[g_optind] + 1;
	if (g_optreset == 1)
		g_optreset = 0;
	g_optopt = *pos++;
	if (!*pos)
		++g_optind;
	if (!(op = ft_strchr(optstring, g_optopt)))
		return (opt_error(av[0], optstring, FALSE));
	g_optarg = NULL;
	if (*(op + 1) == ':' && !set_arg(ac, av, &pos, op))
		return (opt_error(av[0], optstring, TRUE));
	return (g_optopt);
}

int				ft_getopt(int ac, char *const av[], const char *optstring)
{
	if (g_optind >= ac)
		return (-1);
	if (*av[g_optind] != '-' || !*(av[g_optind] + 1))
	{
		if (*optstring != '-')
			return (-1);
		g_optarg = av[g_optind++];
		return ((g_optopt = '\1'));
	}
	if (*(av[g_optind] + 1) == '-')
	{
		g_optopt = '-';
		++g_optind;
		return (-1);
	}
	return (process_arg(ac, av, optstring));
}
