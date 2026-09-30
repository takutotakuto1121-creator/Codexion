/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils2_queue.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tsugimot <tsugimot@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/30 19:31:19 by tsugimot          #+#    #+#             */
/*   Updated: 2026/09/30 19:31:21 by tsugimot         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	init_queue(t_queue *queue)
{
	queue->front = NULL;
	queue->rear = NULL;
}

int	is_empty_queue(t_queue *queue)
{
	return (queue->front == NULL);
}

void	enqueue(t_queue *queue, int data)
{
	t_node	*node;

	node = (t_node *)malloc(sizeof(t_node));
	if (!node)
		return ;
	node->data = data;
	node->next = NULL;
	if (is_empty_queue(queue))
	{
		queue->front = node;
		queue->rear = node;
	}
	else
	{
		queue->rear->next = node;
		queue->rear = node;
	}
}

int	dequeue(t_queue *queue)
{
	t_node	*node;
	t_node	*tmp;
	int		data;

	if (is_empty_queue(queue))
		return (ERROR);
	tmp = queue->front->next;
	node = queue->front;
	queue->front = tmp;
	data = node->data;
	if (queue->front == NULL)
		queue->rear = NULL;
	free(node);
	return (data);
}
