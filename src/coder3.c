/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder3.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tsugimot <tsugimot@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/30 19:30:31 by tsugimot          #+#    #+#             */
/*   Updated: 2026/10/27 16:36:12 by tsugimot         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	take_dongle(t_coder *coder)
{
	t_args	*args;

	args = coder->args;
	pthread_mutex_lock(&args->queue_lock);
	if (!is_in_queue(args->queue, coder->id))
		enqueue(args->queue, coder->id);
	pthread_mutex_unlock(&args->queue_lock);
	while (!args->is_finished)
	{
		pthread_mutex_lock(&args->queue_lock);
		if (can_take_dongles(coder))
		{
			pull_specific_data(args->queue, coder->id);
			coder->dongle[coder->id - 1].busy = TRUE;
			coder->dongle[coder->id % args->number_of_coders].busy = TRUE;
			pthread_mutex_unlock(&args->queue_lock);
			print_status(coder, "has taken a dongle", &args->print_lock);
			print_status(coder, "has taken a dongle", &args->print_lock);
			return (TRUE);
		}
		pthread_mutex_unlock(&args->queue_lock);
		usleep(500);
	}
	return (FALSE);
}
