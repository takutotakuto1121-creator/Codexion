#include <stdio.h>
#include <pthread.h>

// ---------- 失敗例 -------------

// int	count = 0;

// void	*func(void	*p)
// {
// 	for (int i = 0; i < 10000; i++)
// 		count++;
// 	return (NULL);
// }

// int	main(void)
// {
// 	pthread_t	t1, t2;
// 	pthread_create(&t1, NULL, func, NULL);
// 	pthread_create(&t2, NULL, func, NULL);
// 	pthread_join(t1, NULL);
// 	pthread_join(t2, NULL);
// 	printf("count: %d\n", count);
// }

// ---------- 失敗例 ---------------

// ---------- mutex_lock での回避 ----------------

int	count = 0;
pthread_mutex_t	mutex;

void	*func(void	*p)
{
	for (int i = 0; i < 10000; i++)
	{
		pthread_mutex_lock(&mutex);
		count++;
		pthread_mutex_unlock(&mutex);
	}
	return (NULL);
}

int	main(void)
{
	pthread_t	t1, t2;
	pthread_mutex_init(&mutex, NULL);
	pthread_create(&t1, NULL, func, NULL);
	pthread_create(&t2, NULL, func, NULL);
	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
	pthread_mutex_destroy(&mutex);
	printf("count: %d\n", count);
}