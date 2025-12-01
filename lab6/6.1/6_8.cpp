#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void *thread1_func(void *arg) {
    long val = (long)arg;
    long result = val * 2;      
    return (void *)result;      
}

void *thread2_func(void *arg) {
    long val = (long)arg;
    long result = val * 3;      
    return (void *)result;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Использование: %s число\n", argv[0]);
        return 1;
    }

    long input = atol(argv[1]);     
    pthread_t t1, t2;
    void *ret1;
    void *ret2;

    pthread_create(&t1, NULL, thread1_func, (void *)input);
    pthread_join(t1, &ret1);
    long after_thread1 = (long)ret1;

    pthread_create(&t2, NULL, thread2_func, (void *)after_thread1);
    pthread_join(t2, &ret2);
    long final_result = (long)ret2;

    printf("Итог: %ld\n", final_result);

    return 0;
}
