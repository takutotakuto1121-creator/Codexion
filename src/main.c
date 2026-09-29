#include "codexion.h"

int	codexion(t_args *args)
{
	t_coder		*coder;
	t_dongle	*dongle;
	pthread_t	*threads;
	int			i;

	coder = malloc(sizeof(t_coder) * args->number_of_coders);
	dongle = malloc(sizeof(t_dongle) * args->number_of_coders);
	threads = malloc(sizeof(pthread_t) * (args->number_of_coders + 1));

	pthread_mutex_init(&args->print_lock, NULL);
	pthread_mutex_init(&args->queue_lock, NULL);
	args->queue = malloc(sizeof(t_queue));
	init_queue(args->queue);
	args->start_time = get_current_time_ms();
	args->is_finished = False;


	i = 0;
	while (i < args->number_of_coders)
	{
		dongle[i].id = i + 1;
		pthread_mutex_init(&dongle[i].lock, NULL);
		i++;
	}

	i = 0;
	while (i < args->number_of_coders)
	{
		coder[i].id = i + 1;
		coder[i].num_compiled = 0;
		coder[i].last_time_compiled = args->start_time;
		coder[i].args = args;
		coder[i].dongle = dongle;
		pthread_create(&threads[i], NULL, coder_routine, &coder[i]);
		i++;
	}

	pthread_create(&threads[i], NULL, monitor_routine, coder);
	pthread_join(threads[i], NULL);

	i = 0;
	while(i < args->number_of_coders)
	{
		pthread_join(threads[i], NULL);
		i++;
	}

	return (0);
}

int	main(int ac, char **av)
{
	t_args *args;
	if (ac != 9)
	{
		print_error("you must implement 8 arguments!\n");
		return (ERROR);
	}
	args = (t_args *)malloc(sizeof(t_args));
	if (set_args(args, av) == ERROR)
	{
		free(args);
		return (ERROR);
	}
	codexion(args);
	free(args);
	return (0);
}