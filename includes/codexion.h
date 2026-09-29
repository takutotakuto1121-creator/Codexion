#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <pthread.h>

# ifndef ERROR
#define ERROR -1
# endif

# ifndef True
#define True 1
# endif

# ifndef False
#define False 0
# endif

typedef struct      s_node
{
    int             data;
    struct s_node   *next;
}                   t_node;

typedef struct      s_queue
{
    t_node          *front;
    t_node          *rear;
}                   t_queue;

typedef struct      s_args
{
    int             number_of_coders;
    int             time_to_burn_out;
	int             time_to_compile;
    int             time_to_debug;
	int             time_to_refactor;
    int             number_of_compiles_required;
	int             dongle_cooldown;
    char            scheduler[5];
    long long       start_time;
    int             is_finished;
    t_queue         *queue;
    pthread_mutex_t queue_lock;
    pthread_mutex_t print_lock;
}                   t_args;

typedef struct      s_dongle
{
    int             id;
    long long       cooldown_until;
    pthread_mutex_t lock;
}                   t_dongle;

typedef struct      s_coder
{
    int             id;
    int             num_compiled;
    long long       last_time_compiled;
    t_args          *args;
    t_dongle        *dongle;
}                   t_coder;


/* src/parse.c */
int	        is_valid_numbers(char **av);
int	        validate_details(t_args *args);
int	        set_args(t_args *args, char **av);

/* src/coder.c */
void	    print_status(t_coder *coder, char *status, pthread_mutex_t *mutex);
void	    *coder_routine(void *arg);
void	    take_dongle_fifo(t_coder *coder);
void	    take_dongle_edf(t_coder *coder);


/* src/main.c */
int	        codexion(t_args *args);

/* src/monitor.c */
void	    *monitor_routine(void *arg);

/* utils/utils1.c */
int	        ft_strlen(const char *str);
void	    print_error(const char *str);
char	    *ft_strcpy(char *dest, char *src);
long long	get_current_time_ms(void);
int	        get_id_close_to_burnout(t_coder *coder);

/* utils/utils2_queue.c */
void	    init_queue(t_queue *queue);
int		    is_empty_queue(t_queue *queue);
void	    enqueue(t_queue *queue, int data);
int	        dequeue(t_queue *queue);
int	        detect_queue_min(t_queue *queue);
int	        pull_specific_data(t_queue *queue, int data);