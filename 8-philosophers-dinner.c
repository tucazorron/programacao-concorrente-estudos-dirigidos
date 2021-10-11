#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

// define numero de filosofos
#define philosophers 5

// define estado de 'pensando'
#define thinking 0

// define estado de 'com fome'
#define hungry 1

// define estado de 'comendo'
#define eating 2

// inicializa estados dos filosofos
int states[philosophers];

// cria o mutex
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

// cria o semaforo
sem_t semaphore[philosophers];

// funcao para pegar o filosofo a esquerda
int left(int id)
{
	return (id + philosophers - 1) % philosophers;
}

// funcao para pegar o filosofo a direita
int right(int id)
{
	return (id + 1) % philosophers;
}

// analisa os filosfosos adjacentes para atualizar estado
void analyse_philosophers(int n)
{
	// se o filosofo esta com fome e os adjacentes nao estao comendo
	if (states[n] == hungry && states[left(n)] != eating && states[right(n)] != eating)
	{
		// filosofo come
		states[n] = eating;
		// sinaliza que o filosofo pode comer
		sem_post(&semaphore[n]);
	}
}

// pega talheres e se prepara para comer
void get_cutlery(int n)
{
	// da lock
	pthread_mutex_lock(&lock);

	// atualiza o estado do filosofo para 'com fome'
	states[n] = hungry;

	// analisa os filosofos adjacentes
	analyse_philosophers(n);

	// da unlock
	pthread_mutex_unlock(&lock);

	// espera sinal para comecar a comer
	sem_wait(&semaphore[n]);
}

// devolve talheres e termina de comer
void return_cutlery(int n)
{
	// da lock
	pthread_mutex_lock(&lock);

	// atualiza estado do filosofo para pensando
	states[n] = thinking;

	// analysa filosofo da esquerda
	analyse_philosophers(left(n));

	// analisa filosofo da direita
	analyse_philosophers(right(n));

	// da unlock
	pthread_mutex_unlock(&lock);
}

// funcao dos filosofos
void *philosopher(void *arg)
{
	// recebe id do filosofo
	int id = *((int *)arg);

	// loop infinito
	while (1)
	{
		// pensa
		printf("filosofo %d >> pensando\n", id);
		sleep(3);

		// pega talheres
		get_cutlery(id);

		// come
		printf("filosofo %d >> comendo\n", id);
		sleep(3);

		// para de comer
		printf("filosofo %d >> parou de comer\n", id);

		// devolve talheres
		return_cutlery(id);
	}
}

int main()
{
	int i;
	int *id;

	// cria semaforos
	for (i = 0; i < philosophers; i++)
	{
		sem_init(&semaphore[i], 0, 0);
	}

	// inicializa threads
	pthread_t threads[philosophers];

	// cria threads de filofosos
	for (i = 0; i < philosophers; i++)
	{
		id = (int *)malloc(sizeof(int));
		*id = i;
		pthread_create(&threads[i], NULL, philosopher, (void *)(id));
	}

	pthread_join(threads[0], NULL);
	return 0;
}