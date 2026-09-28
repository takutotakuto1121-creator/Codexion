#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <pthread.h>

# ifndef NUM_THREAD
#define NUM_THREAD 8
# endif

# ifndef ERROR
#define ERROR -1
# endif

typedef struct s_args
{
    int number_of_coders,
    int time_to_burn_out,
	int time_to_compile,
    int time_to_debug,
	int time_to_refactor,
    int number_of_copiles_required,
	int dongle_cooldown,
    char *scheduler
} t_args;


/* parse/parse.c */
int	    is_valid_numbers(const char **av);
int	    validate_details(t_args *args);
int	    set_args(t_args *args, const char **av)

/* utils/utils1.c */
int	    ft_strlen(const char *str)
void	print_error(const char *str)

