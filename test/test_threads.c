#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#ifndef NUM
#define NUM 10
#endif

#ifndef NUM_THREAD
#define NUM_THREAD 1
#endif

struct data {
	int	start;		// 計算を始める番号
	int	num;		// 計算する回数
	int	*result;	// 計算結果を書き込む配列
};

int	*single_thread(void)
{
	int	i;
	int	*r;

	i = 0;
	r = (int *)malloc(sizeof(int) * (NUM));
	while (i < NUM)
	{
		r[i] = i * i * i * i;
		i++;
		sleep(1);
	}
	return (r);
}

void	*calculate(void *arg)
{
	int			i;
	struct data	*pd;

	pd = (struct data *)arg;
	i = pd->start;
	while (i < pd->start + pd->num)
	{
		pd->result[i] = i * i * i * i;
		i++;
		sleep(1);
	}
	return NULL;
}

int	*multi_thread(void)
{
	int			i;
	int			*r;
	pthread_t	*t;
	struct data	*d;
	int			chunk;
	int			remainder;
	int			current_start;

	r = (int *)malloc(sizeof(int) * NUM);
	t = (pthread_t *)malloc(sizeof(pthread_t) * NUM_THREAD);
	d = (struct data *)malloc(sizeof(struct data) * NUM_THREAD);
	i = 0;
	chunk = NUM / NUM_THREAD;
	remainder = NUM % NUM_THREAD;
	current_start = 0;

	while (i < NUM_THREAD)
	{
		d[i].start = current_start;
		if (i == NUM_THREAD - 1)
			d[i].num = chunk + remainder;
		else
			d[i].num = chunk;
		d[i].result = r;
		current_start += d[i].num;
		pthread_create(&t[i], NULL, calculate, &d[i]);
		i++;
	}

	i = 0;
	while (i < NUM_THREAD)
	{
		pthread_join(t[i], NULL);
		i++;
	}

	return r;
}




int	main(void)
{
	struct timeval	start;
	struct timeval	end;
	double			time;
	int				*r;
	int				i;

	gettimeofday(&start, NULL);
	r = multi_thread();	// change_this
	gettimeofday(&end, NULL);
	time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 10000000;

	i = 0;
	printf("=== test() ===\n");
	printf("NUM: %d\n", NUM);
	printf("NUM_THREAD: %d\n", NUM_THREAD);
	// while (i < NUM)
	// {
	// 	printf("%d, ", r[i]);
	// 	i++;
	// }
	printf("\n");
	printf("time: %f\n\n", time);
}