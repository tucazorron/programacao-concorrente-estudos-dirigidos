#include "pthread.h"
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"

// numero de threads
#define number_of_threads 10

// cria mutex
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

// contador
int counter = 0;

// funcao de contador
void *counter_function(void *arg)
{
    // trata o id da thread
    int id = *(int *)(arg);

    // cria thread
    printf("thread [%02d] >> criada\n", id);

    // loop infinito
    while (1)
    {
        // da lock
        pthread_mutex_lock(&lock);

        if (counter < 100)
        {
            // mostra o contador
            printf("thread [%02d] >> soma [%04d]\n", id, counter);

            // incrementa contador
            counter++;
        }
        else
        {
            // da unlock
            pthread_mutex_unlock(&lock);

            break;
        }

        // da unlock
        pthread_mutex_unlock(&lock);
    }
}

int main()
{
    // cria threads
    pthread_t threads[number_of_threads];

    int i;
    int *id;

    for (i = 0; i < number_of_threads; i++)
    {
        id = (int *)malloc(sizeof(int));
        *id = i;

        // cria threads na funcao de contador
        pthread_create(&threads[i], NULL, counter_function, (void *)(id));
    }

    for (i = 0; i < number_of_threads; i++)
    {
        // junta as threads
        pthread_join(threads[i], NULL);
    }
    return 0;
}