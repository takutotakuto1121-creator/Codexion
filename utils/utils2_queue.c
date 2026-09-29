#include "codexion.h"

void	init_queue(t_queue *queue)
{
	queue->front = NULL;
	queue->rear = NULL;
}

int		is_empty_queue(t_queue *queue)
{
	return (queue->front == NULL);
}

void	enqueue(t_queue *queue, int data)
{
	t_node	*node;

	node = (t_node *)malloc(sizeof(t_node));
	if (!node)
		return;
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
	t_node *node;
	t_node *tmp;
	int		data;

	if (is_empty_queue(queue))
		return(ERROR);
	tmp = queue->front->next;
	node = queue->front;
	queue->front = tmp;
	data = node->data;
	if (queue->front == NULL)
		queue->rear = NULL;
	free(node);
	return (data);
}

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
	while(node)
	{
		if (node->data == data)
		{
			prev->next = node->next;
			if (node->next == NULL)
				queue->rear = prev;
			free(node);
			return (data);
		}
		prev = node;
		node = node->next;
	}
	return (ERROR);
}