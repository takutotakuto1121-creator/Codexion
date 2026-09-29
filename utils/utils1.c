#include "codexion.h"

int	ft_strlen(const char *str)
{
	int	i;

	i = 0;
	while(str[i])
		i++;
	return (i);
}

void	print_error(const char *str)
{
	int	i;

	i = ft_strlen(str);
	write(2, "[ERROR]", 7);
	write(2, str, i);
}

char	*ft_strcpy(char *dest, char *src)
{
	int	i;

	i = 0;
	while(src[i])
	{
		dest[i] = src[i];
		i++;
	}
	dest[i] = '\0';
	return (dest);
}

long long	get_current_time_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((long long)tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

int	get_id_close_to_burnout(t_coder *coder)
{
	t_queue		*queue;
	t_node		*node;
	t_coder		*coders;
	int			id;
	long long	oldest_time;

	if (is_empty_queue(coder->args->queue))
		return (ERROR);
	coders = coder - (coder->id - 1);
	queue = coders->args->queue;
	node = queue->front;
	id = node->data;
	oldest_time = coders[id - 1].last_time_compiled;

	while (node)
	{
		if (coders[node->data - 1].last_time_compiled < oldest_time)
		{
			oldest_time = coders[node->data - 1].last_time_compiled;
			id = node->data;
		}
		node = node->next;
	}
	return (id);
}