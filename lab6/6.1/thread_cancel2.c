#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void *worker(void *arg) {
    printf("Поток: начинаю работу\n");

    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);

    for (int i = 0; i < 10; i++) {
        sleep(1);                  
        printf("Поток: шаг %d\n", i);
    }

    printf("Поток: завершился сам\n");

    return NULL;
}

int main(void) {
    pthread_t tid;

    pthread_create(&tid, NULL, worker, NULL);

    sleep(3);
    printf("Главный поток: отправляю pthread_cancel\n");
    pthread_cancel(tid);

    pthread_join(tid, NULL);

    printf("Главный поток: программа завершена\n");
    return 0;
}
