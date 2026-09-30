/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tsugimot <tsugimot@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/30 19:30:31 by tsugimot          #+#    #+#             */
/*   Updated: 2026/09/30 19:30:36 by tsugimot         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	print_status(t_coder *coder, char *status, pthread_mutex_t *mutex)
{
	long long	current_time;
	long long	elapsed_time;

	current_time = get_current_time_ms();
	elapsed_time = current_time - coder->args->start_time;
	pthread_mutex_lock(mutex);
	printf("%lld %d %s\n", elapsed_time, coder->id, status);
	pthread_mutex_unlock(mutex);
}

void	wait_and_lock_dongles(t_coder *coder, int left, int right)
{
	long long	now;

	now = get_current_time_ms();
	if (now < coder->dongle[left].cooldown_until)
		usleep((coder->dongle[left].cooldown_until - now) * 1000);
	now = get_current_time_ms();
	if (now < coder->dongle[right].cooldown_until)
		usleep((coder->dongle[right].cooldown_until - now) * 1000);
	if (coder->id == coder->args->number_of_coders)
	{
		pthread_mutex_lock(&coder->dongle[right].lock);
		pthread_mutex_lock(&coder->dongle[left].lock);
	}
	else
	{
		pthread_mutex_lock(&coder->dongle[left].lock);
		pthread_mutex_lock(&coder->dongle[right].lock);
	}
}

void	take_dongle_fifo(t_coder *coder)
{
	int	left;
	int	right;

	left = coder->id - 1;
	right = coder->id % coder->args->number_of_coders;
	pthread_mutex_lock(&coder->args->queue_lock);
	enqueue(coder->args->queue, coder->id);
	pthread_mutex_unlock(&coder->args->queue_lock);
	while (1)
	{
		if (coder->args->is_finished)
			return ;
		pthread_mutex_lock(&coder->args->queue_lock);
		if (!is_empty_queue(coder->args->queue)
			&& coder->args->queue->front->data == coder->id)
		{
			dequeue(coder->args->queue);
			pthread_mutex_unlock(&coder->args->queue_lock);
			wait_and_lock_dongles(coder, left, right);
			return ;
		}
		pthread_mutex_unlock(&coder->args->queue_lock);
		usleep(500);
	}
}

void	take_dongle_edf(t_coder *coder)
{
	int	left;
	int	right;

	left = coder->id - 1;
	right = coder->id % coder->args->number_of_coders;
	pthread_mutex_lock(&coder->args->queue_lock);
	enqueue(coder->args->queue, coder->id);
	pthread_mutex_unlock(&coder->args->queue_lock);
	while (1)
	{
		if (coder->args->is_finished)
			return ;
		pthread_mutex_lock(&coder->args->queue_lock);
		if (!is_empty_queue(coder->args->queue)
			&& coder->id == get_id_close_to_burnout(coder))
		{
			pull_specific_data(coder->args->queue, coder->id);
			pthread_mutex_unlock(&coder->args->queue_lock);
			wait_and_lock_dongles(coder, left, right);
			return ;
		}
		pthread_mutex_unlock(&coder->args->queue_lock);
		usleep(500);
	}
}
