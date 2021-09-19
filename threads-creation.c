#include "stdio.h"
#include "stdlib.h"
#include "pthread.h"
#include "unistd.h"

#define N 10

pthread_mutex_t threads = PTHREAD_MUTEX_INITIALIZER;
pthread_t a[N];
int counter = 0;
int flag = 1;

void *pthread_func(void *id)
{
    printf("\nTHREAD  =  %02d | CRIADA\n\n", *(int *)(id));

    while (flag)
    {
        pthread_mutex_lock(&threads);
        if (counter < 1000)
        {
            counter++;
            printf("COUNTER =  %04d | THREAD = %02d\n", counter, *(int *)(id));
        }
        else
        {
            flag = 0;
        }
        pthread_mutex_unlock(&threads);
    }
}

int main()
{
    printf("ID: %lu\n", pthread_self());
    int i;
    int *id;
    for (i = 0; i < N; i++)
    {
        id = (int *)malloc(sizeof(int));
        *id = i;
        pthread_create(&a[i], NULL, pthread_func, (void *)(id));
    }
    for (i = 0; i < N; i++)
    {
        pthread_join(a[i], NULL);
    }
    printf("TERMINANDO\n");
    return 0;
}