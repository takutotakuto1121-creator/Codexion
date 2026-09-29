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

void	take_dongle_fifo(t_coder *coder)
{
	int	left;
	int	right;

	left = coder->id - 1;
	right = coder->id % coder->args->number_of_coders;

	pthread_mutex_lock(&coder->args->queue_lock);
	enqueue(coder->args->queue, coder->id);
	pthread_mutex_unlock(&coder->args->queue_lock);

	while(1)
	{
		if (coder->args->is_finished)
			return;

		pthread_mutex_lock(&coder->args->queue_lock);
		if (!is_empty_queue(coder->args->queue) && coder->args->queue->front->data == coder->id)
		{	
			dequeue(coder->args->queue);
			pthread_mutex_unlock(&coder->args->queue_lock);

			if (get_current_time_ms() < coder->dongle[left].cooldown_until)
				usleep((coder->dongle[left].cooldown_until - get_current_time_ms()) * 1000);
			if (get_current_time_ms() < coder->dongle[right].cooldown_until)
				usleep((coder->dongle[right].cooldown_until - get_current_time_ms()) * 1000);

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
			return;
		}
		pthread_mutex_unlock(&coder->args->queue_lock);
		usleep(500);
	}
}

void	take_dongle_edf(t_coder *coder)
{
	int		left;
	int		right;

	left = coder->id - 1;
	right = coder->id % coder->args->number_of_coders;

	pthread_mutex_lock(&coder->args->queue_lock);
	enqueue(coder->args->queue, coder->id);
	pthread_mutex_unlock(&coder->args->queue_lock);

	while(1)
	{
		if (coder->args->is_finished)
			return;

		pthread_mutex_lock(&coder->args->queue_lock);
		if (!is_empty_queue(coder->args->queue) && coder->id == get_id_close_to_burnout(coder))
		{	
			pull_specific_data(coder->args->queue, coder->id);
			pthread_mutex_unlock(&coder->args->queue_lock);

			if (get_current_time_ms() < coder->dongle[left].cooldown_until)
				usleep((coder->dongle[left].cooldown_until - get_current_time_ms()) * 1000);
			if (get_current_time_ms() < coder->dongle[right].cooldown_until)
				usleep((coder->dongle[right].cooldown_until - get_current_time_ms()) * 1000);

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
			return;
		}
		pthread_mutex_unlock(&coder->args->queue_lock);
		usleep(500);
	}
}

void	take_dongle(t_coder *coder)
{
		if (strcmp(coder->args->scheduler, "fifo") == 0)
			take_dongle_fifo(coder);
		else if (strcmp(coder->args->scheduler, "edf") == 0)
			take_dongle_edf(coder);
}

void	free_dongle(t_coder *coder)
{
	int	left;
	int	right;

	left = coder->id - 1;
	right = coder->id % coder->args->number_of_coders;

	coder->dongle[left].cooldown_until = get_current_time_ms() + coder->args->dongle_cooldown;
	coder->dongle[right].cooldown_until = get_current_time_ms() + coder->args->dongle_cooldown;

	pthread_mutex_unlock(&coder->dongle[left].lock);
	pthread_mutex_unlock(&coder->dongle[right].lock);
}

void	*coder_routine(void *arg)
{
	t_coder *coder;

	coder = (t_coder *)arg;
	coder->last_time_compiled = get_current_time_ms();
	while(!coder->args->is_finished)
	{
		take_dongle(coder);
		if (coder->args->is_finished)
		{
			free_dongle(coder);
			return (NULL);
		}
		print_status(coder, "has taken a dongle", &coder->args->print_lock);
		print_status(coder, "has taken a dongle", &coder->args->print_lock);

		print_status(coder, "is compiling", &coder->args->print_lock);
		coder->last_time_compiled = get_current_time_ms();
		usleep(coder->args->time_to_compile * 1000);
		coder->num_compiled++;

		free_dongle(coder);

		if (coder->args->is_finished)
			return (NULL);
		print_status(coder, "is_debugging", &coder->args->print_lock);
		usleep(coder->args->time_to_debug * 1000);

		if (coder->args->is_finished)
			return (NULL);
		print_status(coder, "is_refactoring", &coder->args->print_lock);
		usleep(coder->args->time_to_refactor * 1000);
	}
	return (NULL);
}
