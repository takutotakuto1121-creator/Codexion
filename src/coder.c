#include "codexion.h"

void	print_status(t_coder *coder, char *status)
{
	long long	current_time;
	long long	elapsed_time;

	current_time = get_current_time_ms();
	elapsed_time = current_time - coder->args->start_time;

	printf("%lld %d %s\n", elapsed_time, coder->id, status);
}

void	take_dongle(t_coder *coder)
{
	int	left;
	int	right;

	left = coder->id % coder->args->number_of_coders;
	right = coder->id % coder->args->number_of_coders + 1;

	pthread_mutex_lock(&coder->dongle[left].lock);
	pthread_mutex_lock(&coder->dongle[right].lock);
}

void	free_dongle(t_coder *coder)
{
	int	left;
	int	right;

	left = coder->id % coder->args->number_of_coders;
	right = coder->id % coder->args->number_of_coders + 1;

	usleep(coder->args->dongle_cooldown * 1000); // 今の実装だとこの分だけcoderも待つことになってしまう。
	pthread_mutex_unlock(&coder->dongle[left].lock);
	pthread_mutex_unlock(&coder->dongle[right].lock);
}

void	*coder_routine(void *arg)
{
	t_coder *coder;

	coder = (t_coder *)arg;
	while(1)
	{
		take_dongle(coder);
		pthread_mutex_lock(&coder->args->print_lock);
		print_status(coder, "has taken a dongle");
		print_status(coder, "has taken a dongle");
		pthread_mutex_unlock(&coder->args->print_lock);

		pthread_mutex_lock(&coder->args->print_lock);
		print_status(coder, "is compiling");
		pthread_mutex_unlock(&coder->args->print_lock);
		usleep(coder->args->time_to_compile * 1000);

		free_dongle(coder);

		pthread_mutex_lock(&coder->args->print_lock);
		print_status(coder, "is_debugging");
		pthread_mutex_unlock(&coder->args->print_lock);
		usleep(coder->args->time_to_debug * 1000);

		pthread_mutex_lock(&coder->args->print_lock);
		print_status(coder, "is_refactoring");
		pthread_mutex_unlock(&coder->args->print_lock);
		usleep(coder->args->time_to_refactor * 1000);
	}
	return (NULL);
}