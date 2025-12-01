#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int available = 0;

void *worker(void *arg) {
    available++;
    printf("2-й поток: available увеличен до %d\n", available);

    sleep(2);
    printf("2-й поток: работа завершена\n");
    return NULL;
}

int main(void) {
    pthread_t t2;

    printf("Главный поток: создаю 2-й поток\n");
    pthread_create(&t2, NULL, worker, NULL);

    pthread_join(t2, NULL);

    printf("Главный поток: после join, available = %d\n", available);

    sleep(3);
    printf("Главный поток: завершил работу\n");

    return 0;
}
