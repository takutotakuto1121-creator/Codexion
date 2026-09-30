/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tsugimot <tsugimot@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/30 19:31:03 by tsugimot          #+#    #+#             */
/*   Updated: 2026/09/30 19:31:05 by tsugimot         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	check_burnout(t_coder *coders, t_args *args)
{
	int	i;

	i = 0;
	while (i < args->number_of_coders)
	{
		if ((get_current_time_ms() - coders[i].last_time_compiled)
			> args->time_to_burn_out)
		{
			args->is_finished = TRUE;
			print_status(&coders[i], "burned out", &args->print_lock);
			return (1);
		}
		i++;
	}
	return (0);
}

static int	check_finished(t_coder *coders, t_args *args)
{
	int	i;
	int	finish_count;

	i = 0;
	finish_count = 0;
	while (i < args->number_of_coders)
	{
		if (coders[i].num_compiled >= args->number_of_compiles_required)
			finish_count++;
		i++;
	}
	if (finish_count == args->number_of_coders)
		args->is_finished = TRUE;
	return (args->is_finished);
}

void	*monitor_routine(void *arg)
{
	t_coder	*coders;
	t_args	*args;

	coders = (t_coder *)arg;
	args = coders[0].args;
	while (!args->is_finished)
	{
		if (check_burnout(coders, args))
			return (NULL);
		if (args->is_finished)
			break ;
		if (check_finished(coders, args))
			break ;
	}
	return (NULL);
}
