/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main2.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tsugimot <tsugimot@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/30 19:30:57 by tsugimot          #+#    #+#             */
/*   Updated: 2026/10/19 19:07:58 by tsugimot         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	enqueue_initial_order(t_args *args)
{
	int	id;

	id = 1;
	while (id <= args->number_of_coders)
	{
		enqueue(args->queue, id);
		id += 2;
	}
	id = 2;
	while (id <= args->number_of_coders)
	{
		enqueue(args->queue, id);
		id += 2;
	}
}

void	init_coders(t_coder *c, t_dongle *d, t_args *args, pthread_t *th)
{
	int	i;

	enqueue_initial_order(args);
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
		dongle[i].cooldown_until = 0;
		dongle[i].busy = FALSE;
		i++;
	}
}
