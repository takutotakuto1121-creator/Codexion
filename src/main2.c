/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main2.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tsugimot <tsugimot@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/30 19:30:57 by tsugimot          #+#    #+#             */
/*   Updated: 2026/09/30 19:30:59 by tsugimot         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	init_coders(t_coder *c, t_dongle *d, t_args *args, pthread_t *th)
{
	int	i;

	i = 0;
	while (i < args->number_of_coders)
	{
		c[i].id = i + 1;
		c[i].num_compiled = 0;
		c[i].last_time_compiled = args->start_time;
		c[i].args = args;
		c[i].dongle = d;
		pthread_create(&th[i], NULL, coder_routine, &c[i]);
		i++;
	}
}

void	init_dongles(t_dongle *dongle, t_args *args)
{
	int	i;

	i = 0;
	while (i < args->number_of_coders)
	{
		dongle[i].id = i + 1;
		pthread_mutex_init(&dongle[i].lock, NULL);
		i++;
	}
}
