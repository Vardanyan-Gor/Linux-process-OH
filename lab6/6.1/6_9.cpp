#include <stdio.h>
#include <pthread.h>

int data[10];  

void *fill_even(void *arg) {

    for (int i = 0; i < 10; i += 2) {
        data[i] = i + 1;   
    }
    return NULL;
}

int main(void) {
    pthread_t tid;


    pthread_create(&tid, NULL, fill_even, NULL);


    for (int i = 1; i < 10; i += 2) {
        data[i] = i + 1;   
    }

    pthread_join(tid, NULL);

    for (int i = 0; i < 10; i++) {
        printf("%d ", data[i]);
    }
    printf("\n");

    return 0;
}
