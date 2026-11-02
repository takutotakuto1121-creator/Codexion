/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tsugimot <tsugimot@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/30 19:30:31 by tsugimot          #+#    #+#             */
/*   Updated: 2026/10/27 16:36:12 by tsugimot         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	print_status(t_coder *coder, char *status, pthread_mutex_t *mutex)
{
	pthread_mutex_lock(mutex);
	if (!coder->args->is_finished)
		printf("%lld %d %s\n", get_current_time_ms() - coder->args->start_time,
			coder->id, status);
	pthread_mutex_unlock(mutex);
}

static int	shares_dongle(t_coder *coder, int other)
{
	int	n;
	int	l1;
	int	r1;

	n = coder->args->number_of_coders;
	l1 = coder->id - 1;
	r1 = coder->id % n;
	return (l1 == other - 1 || l1 == other % n
		|| r1 == other - 1 || r1 == other % n);
}

static int	is_ahead(t_coder *me, int other, int seen_me)
{
	t_coder	*o;

	if (strcmp(me->args->scheduler, "fifo") == 0)
		return (!seen_me);
	o = me - (me->id - 1) + (other - 1);
	if (o->last_time_compiled != me->last_time_compiled)
		return (o->last_time_compiled < me->last_time_compiled);
	return (!seen_me);
}

static int	has_rival(t_coder *coder)
{
	t_node	*node;
	int		seen_me;

	node = coder->args->queue->front;
	seen_me = FALSE;
	while (node)
	{
		if (node->data == coder->id)
			seen_me = TRUE;
		else if (shares_dongle(coder, node->data)
			&& is_ahead(coder, node->data, seen_me))
			return (TRUE);
		node = node->next;
	}
	return (FALSE);
}

int	can_take_dongles(t_coder *coder)
{
	t_dongle	*left;
	t_dongle	*right;
	long long	now;

	left = &coder->dongle[coder->id - 1];
	right = &coder->dongle[coder->id % coder->args->number_of_coders];
	now = get_current_time_ms();
	if (left->busy || right->busy
		|| now < left->cooldown_until || now < right->cooldown_until)
		return (FALSE);
	return (!has_rival(coder));
}
