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

# ifndef True
#define True 1
# endif

# ifndef False
#define False 0
# endif

typedef struct      s_args
{
    int             number_of_coders;
    int             time_to_burn_out;
	int             time_to_compile;
    int             time_to_debug;
	int             time_to_refactor;
    int             number_of_copiles_required;
	int             dongle_cooldown;
    char            scheduler[5];
    long long       start_time;
    pthread_mutex_t print_lock;
}                   t_args;

typedef struct      s_dongle
{
    int             id;
    pthread_mutex_t lock;
}                   t_dongle;

typedef struct      s_coder
{
    int             id;
    t_args          *args;
    t_dongle        *dongle;
}                   t_coder;


/* src/parse.c */
int	        is_valid_numbers(char **av);
int	        validate_details(t_args *args);
int	        set_args(t_args *args, char **av);

/* src/coder.c */
void	    print_status(t_coder *coder, char *status);
void	    *coder_routine(void *arg);

/* src/main.c */
int	        codexion(t_args *args);

/* src/monitor.c */


/* utils/utils1.c */
int	        ft_strlen(const char *str);
void	    print_error(const char *str);
char	    *ft_strcpy(char *dest, char *src);
long long	get_current_time_ms(void);
