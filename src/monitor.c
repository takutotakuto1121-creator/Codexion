#include "codexion.h"

void	*monitor_routine(void *arg)
{
	t_coder	*coders;
	t_args	*args;
	int		i;
	int		finish_count;

	coders = (t_coder *)arg;
	args = coders[0].args;
	while (!args->is_finished)
	{
		i = 0;
		finish_count =0;

		while (i < args->number_of_coders)
		{
			if ((get_current_time_ms() - coders[i].last_time_compiled) > args->time_to_burn_out)
			{
				args->is_finished = True;
				print_status(&coders[i], "burned out", &args->print_lock);
				return (NULL);
			}
			i++;
		}

		if (args->is_finished)
			break;

		i = 0;
		while(i < args->number_of_coders)
		{
			if (coders[i].num_compiled >= args->number_of_compiles_required)
				finish_count++;
			i++;
		}

		if (finish_count == args->number_of_coders)
			args->is_finished = True;
	}
	return NULL;
}