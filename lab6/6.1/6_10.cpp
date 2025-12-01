#include <stdio.h>
#include <pthread.h>

int data[10] = {1,2,3,4,5, 6,7,8,9,10};

void *sum_first(void *arg) {
    long sum = 0;
    for (int i = 0; i < 5; i++) {
        sum += data[i];
    }
    return (void *)sum;
}

void *sum_last(void *arg) {
    long sum = 0;
    for (int i = 5; i < 10; i++) {
        sum += data[i];
    }
    return (void *)sum;
}

int main(void) {
    pthread_t t1, t2;
    void *r1, *r2;

    pthread_create(&t1, NULL, sum_first, NULL);
    pthread_create(&t2, NULL, sum_last, NULL);

    pthread_join(t1, &r1);
    pthread_join(t2, &r2);

    long sum1 = (long)r1;
    long sum2 = (long)r2;
    long total = sum1 + sum2;

    printf("Сумма первых 5 элементов: %ld\n", sum1);
    printf("Сумма последних 5 элементов: %ld\n", sum2);
    printf("Общая сумма: %ld\n", total);

    return 0;
}
