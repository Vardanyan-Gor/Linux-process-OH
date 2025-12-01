#include <stdio.h>
#include <pthread.h>
#include <string.h>

void *thread_func(void *arg) {
    char *str = (char *)arg;
    size_t len = strlen(str);
    return (void *)len;  
}

int main(void) {
    pthread_t tid;
    const char *text = "Пример строки";
    void *ret;         

    if (pthread_create(&tid, NULL, thread_func, (void *)text) != 0) {
        perror("pthread_create");
        return 1;
    }


    if (pthread_join(tid, &ret) != 0) {
        perror("pthread_join");
        return 1;
    }

    long length = (long)ret;  

    printf("Длина строки: %ld\n", length);

    return 0;
}
