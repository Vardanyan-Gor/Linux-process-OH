#include <stdio.h>
#include <pthread.h>

void *thread_func(void *arg) {
    pthread_t inside = pthread_self();  
    pthread_t outside = *(pthread_t *)arg; 

    printf("pthread_equal: %d\n", pthread_equal(inside, outside));
    printf("== оператор:   %d\n", inside == outside);

    return NULL;
}

int main(void) {
    pthread_t tid;

    pthread_create(&tid, NULL, thread_func, &tid);
    pthread_join(tid, NULL);

    return 0;
}
