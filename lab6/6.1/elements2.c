#include <stdio.h>
#include <pthread.h>

int data[] = {1, 2, 3, 4, 5};
int size = 5;


__thread int index_local = 0;

int getElement(void) {
    if (index_local >= size) {
        return -1;
    }
    int v = data[index_local];
    index_local++;              
    return v;
}

void *thread_func(void *arg) {
    long id = (long)arg;
    int v;

    while ((v = getElement()) != -1) {
        printf("thread %ld: %d\n", id, v);
    }

    return NULL;
}

int main(void) {
    pthread_t t1, t2;

    pthread_create(&t1, NULL, thread_func, (void *)1);
    pthread_create(&t2, NULL, thread_func, (void *)2);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    return 0;
}

