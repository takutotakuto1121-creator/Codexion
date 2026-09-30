/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils3_queue.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tsugimot <tsugimot@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/30 19:31:24 by tsugimot          #+#    #+#             */
/*   Updated: 2026/09/30 19:31:26 by tsugimot         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	detect_queue_min(t_queue *queue)
{
	t_node	*node;
	int		min;

	if (is_empty_queue(queue))
		return (ERROR);
	node = queue->front;
	min = node->data;
	while (node)
	{
		if (node->data < min)
			min = node->data;
		node = node->next;
	}
	return (min);
}

static int	remove_node(t_queue *q, t_node *p, t_node *n, int d)
{
	p->next = n->next;
	if (n->next == NULL)
		q->rear = p;
	free(n);
	return (d);
}

int	pull_specific_data(t_queue *queue, int data)
{
	t_node	*node;
	t_node	*prev;

	if (is_empty_queue(queue))
		return (ERROR);
	node = queue->front;
	if (node->data == data)
	{
		dequeue(queue);
		return (data);
	}
	prev = node;
	node = node->next;
	while (node)
	{
		if (node->data == data)
			return (remove_node(queue, prev, node, data));
		prev = node;
		node = node->next;
	}
	return (ERROR);
}
