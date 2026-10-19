/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder2.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tsugimot <tsugimot@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/30 19:30:42 by tsugimot          #+#    #+#             */
/*   Updated: 2026/10/19 18:55:08 by tsugimot         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	take_dongle(t_coder *coder)
{
	if (strcmp(coder->args->scheduler, "fifo") == 0)
		take_dongle_fifo(coder);
	else if (strcmp(coder->args->scheduler, "edf") == 0)
		take_dongle_edf(coder);
}

void	free_dongle(t_coder *coder)
{
	int			left;
	int			right;
	long long	now;

	left = coder->id - 1;
	right = coder->id % coder->args->number_of_coders;
	now = get_current_time_ms();
	coder->dongle[left].cooldown_until = now + coder->args->dongle_cooldown;
	coder->dongle[right].cooldown_until = now + coder->args->dongle_cooldown;
	pthread_mutex_unlock(&coder->dongle[left].lock);
	pthread_mutex_unlock(&coder->dongle[right].lock);
}

static int	coder_loop_body(t_coder *coder)
{
	take_dongle(coder);
	if (coder->args->is_finished)
	{
		free_dongle(coder);
		return (1);
	}
	print_status(coder, "is compiling", &coder->args->print_lock);
	coder->last_time_compiled = get_current_time_ms();
	usleep(coder->args->time_to_compile * 1000);
	coder->num_compiled++;
	free_dongle(coder);
	if (coder->args->is_finished)
		return (1);
	print_status(coder, "is debugging", &coder->args->print_lock);
	usleep(coder->args->time_to_debug * 1000);
	if (coder->args->is_finished)
		return (1);
	print_status(coder, "is refactoring", &coder->args->print_lock);
	usleep(coder->args->time_to_refactor * 1000);
	return (0);
}

static int	coder_num_1(t_coder *coder)
{
	if (coder->args->number_of_coders != 1)
		return (0);
	print_status(coder, "has taken a dongle", &coder->args->print_lock);
	while (1)
	{
		if (coder->args->is_finished)
			return (1);
	}
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	coder->last_time_compiled = get_current_time_ms();
	if (coder_num_1(coder))
		return (NULL);
	while (!coder->args->is_finished)
	{
		if (coder_loop_body(coder))
			return (NULL);
	}
	return (NULL);
}
