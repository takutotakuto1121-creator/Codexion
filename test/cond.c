#include <pthread.h>

pthread_mutex_t mutex;
pthread_cond_t cond;
int is_ready = 0;

void *wait_thread(void *arg) {
    pthread_mutex_lock(&mutex);
    
    while (is_ready == 0) {
        pthread_cond_wait(&cond, &mutex);
    }
    
    pthread_mutex_unlock(&mutex);
    return NULL;
}

void *signal_thread(void *arg) {
    pthread_mutex_lock(&mutex);
    
    is_ready = 1;
    pthread_cond_signal(&cond);
    
    pthread_mutex_unlock(&mutex);
    return NULL;
}

int main() {
    pthread_t t1, t2;

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    pthread_create(&t1, NULL, wait_thread, NULL);
    pthread_create(&t2, NULL, signal_thread, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    return 0;
}