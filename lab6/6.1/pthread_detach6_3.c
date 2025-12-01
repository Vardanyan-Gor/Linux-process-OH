#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void *thread_func(void *arg) {
    printf("Поток запущен и работает...\n");
    sleep(1);
    printf("Поток завершён.\n");
    return NULL;
}

int main(void) {
    pthread_t tid;

    pthread_create(&tid, NULL, thread_func, NULL);

    pthread_detach(tid);

    printf("Главный поток закончился.\n");

    sleep(2);

    return 0;
}
