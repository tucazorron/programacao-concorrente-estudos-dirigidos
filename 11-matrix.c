#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

// define tamanho maximo da matriz
#define max_size 10000

// cria barreira
pthread_barrier_t barrier;

// cria tamanho da matriz
int size;

// cria array de somas parciais por thread
int sums_array[max_size];

// cria a matriz
int matrix[max_size][max_size];

// calcula a soma da matriz
void *matrix_sum(void *arg)
{
    int id = *((int *)arg);

    // inicializa a soma parcial da thread como zero
    int partial_sum = 0;

    int i;
    for (i = 0; i < size; i++)
    {
        // incrementa na soma parcial de cada thread o valor dentro da matriz
        partial_sum += matrix[id][i];
    }

    // guarda dentro da array de somas o valor da soma parcial de cada thread
    sums_array[id] = partial_sum;

    printf("thread [%04d] >> soma parcial =  %d\n", id, partial_sum);

    // espera na barreira ate todas as threads chegarem aqui
    pthread_barrier_wait(&barrier);

    // se for a thread de id 0
    if (id == 0)
    {
        // inicializa total como 0
        int total = 0;

        for (i = 0; i < size; i++)
        {
            // incrementa total com os valores da array de somas
            total += sums_array[i];
        }
        printf("\nsoma total da matriz: %d\n", total);
    }
    pthread_exit(0);
}

int main(int argc, char *argv[])
{
    // coleta o tamanho da matriz como parametro da execucao
    size = atoi(argv[1]);

    // se o tamanho for maior que o maximo, programa encerra
    if (size > max_size)
    {
        printf("erro >> tamanho maior que o permitido pelo programa\n");
        return 0;
    }

    // atualiza a seed de randomizacao pelo tempo
    srand(time(NULL));

    // inicializa a barreira
    pthread_barrier_init(&barrier, NULL, size);

    int i, j;
    printf("\n");
    for (i = 0; i < size; i++)
    {
        printf("| ");
        for (j = 0; j < size; j++)
        {
            matrix[i][j] = rand() % 100;
            printf("%02d | ", matrix[i][j]);
            if (j == size - 1)
            {
                printf("\n");
            }
        }
    }
    printf("\n");

    pthread_t threads[size];

    int *id;

    for (i = 0; i < size; i++)
    {
        id = (int *)malloc(sizeof(int));
        *id = i;
        pthread_create(&threads[i], NULL, matrix_sum, (void *)id);
    }

    for (i = 0; i < size; i++)
    {
        pthread_join(threads[i], NULL);
    }
    return 0;
}
