/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tsugimot <tsugimot@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/30 19:31:09 by tsugimot          #+#    #+#             */
/*   Updated: 2026/09/30 19:31:11 by tsugimot         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	is_valid_numbers(char **av)
{
	int	i;
	int	j;

	j = 1;
	while (j < 8)
	{
		i = 0;
		if (!av[j] || av[j][0] == '\0')
			return (ERROR);
		if (av[j][i] == '+')
			i++;
		while (av[j][i] != '\0')
		{
			if (av[j][i] < '0' || av[j][i] > '9')
				return (ERROR);
			i++;
		}
		j++;
	}
	return (0);
}

int	validate_details(t_args *args)
{
	if (args->number_of_coders < 1)
	{
		print_error("number of coders shoule be more than 1\n");
		return (ERROR);
	}
	if (strcmp(args->scheduler, "fifo") != 0
		&& strcmp(args->scheduler, "edf") != 0)
	{
		print_error("scheduler should be fifo or edf\n");
		return (ERROR);
	}
	return (0);
}

int	set_args(t_args *args, char **av)
{
	if (is_valid_numbers(av) == ERROR)
	{
		print_error("args without scheduler should be positive integers\n");
		return (ERROR);
	}
	args->number_of_coders = atoi(av[1]);
	args->time_to_burn_out = atoi(av[2]);
	args->time_to_compile = atoi(av[3]);
	args->time_to_debug = atoi(av[4]);
	args->time_to_refactor = atoi(av[5]);
	args->number_of_compiles_required = atoi(av[6]);
	args->dongle_cooldown = atoi(av[7]);
	ft_strcpy(args->scheduler, av[8]);
	if (validate_details(args) == ERROR)
		return (ERROR);
	return (0);
}
