#include "codexion.h"

int	codexion(t_args *args)
{
	
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