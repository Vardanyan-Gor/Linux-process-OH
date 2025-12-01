#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int available = 0;

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  cond = PTHREAD_COND_INITIALIZER;

void *worker(void *arg) {
    pthread_mutex_lock(&mtx);
    available++;
    printf("2-й поток: available увеличен до %d\n", available);
    pthread_cond_signal(&cond);     
    pthread_mutex_unlock(&mtx);

    sleep(2);
    printf("2-й поток: работа завершена\n");
    return NULL;
}

int main(void) {
    pthread_t t2;

    printf("Главный поток: создаю 2-й поток\n");
    pthread_create(&t2, NULL, worker, NULL);

    pthread_mutex_lock(&mtx);
    while (available == 0) {        
        pthread_cond_wait(&cond, &mtx);
    }
    printf("Главный поток: получил сигнал, available = %d\n", available);
    pthread_mutex_unlock(&mtx);

    sleep(3);
    printf("Главный поток: завершил работу\n");

    pthread_join(t2, NULL);

    pthread_mutex_destroy(&mtx);
    pthread_cond_destroy(&cond);

    return 0;
}

