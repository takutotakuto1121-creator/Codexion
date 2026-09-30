/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tsugimot <tsugimot@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/30 19:30:51 by tsugimot          #+#    #+#             */
/*   Updated: 2026/09/30 19:30:53 by tsugimot         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	init_resources(t_args *args, t_coder **coder,
				t_dongle **dongle, pthread_t **threads)
{
	*coder = malloc(sizeof(t_coder) * args->number_of_coders);
	if (!*coder)
		return (ERROR);
	*dongle = malloc(sizeof(t_dongle) * args->number_of_coders);
	if (!*dongle)
	{
		free(*coder);
		return (ERROR);
	}
	*threads = malloc(sizeof(pthread_t) * (args->number_of_coders + 1));
	if (!*threads)
	{
		free(*coder);
		free(*dongle);
		return (ERROR);
	}
	return (0);
}

static void	join_all_threads(t_args *args, pthread_t *threads)
{
	int	i;

	pthread_join(threads[args->number_of_coders], NULL);
	i = 0;
	while (i < args->number_of_coders)
	{
		pthread_join(threads[i], NULL);
		i++;
	}
}

int	codexion(t_args *args)
{
	t_coder		*coder;
	t_dongle	*dongle;
	pthread_t	*threads;

	if (init_resources(args, &coder, &dongle, &threads) == ERROR)
		return (ERROR);
	pthread_mutex_init(&args->print_lock, NULL);
	pthread_mutex_init(&args->queue_lock, NULL);
	args->queue = malloc(sizeof(t_queue));
	init_queue(args->queue);
	args->start_time = get_current_time_ms();
	args->is_finished = FALSE;
	init_dongles(dongle, args);
	init_coders(coder, dongle, args, threads);
	pthread_create(&threads[args->number_of_coders], NULL,
		monitor_routine, coder);
	join_all_threads(args, threads);
	free(coder);
	free(dongle);
	free(threads);
	return (0);
}

int	main(int ac, char **av)
{
	t_args	*args;

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
