#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>

// cria mutex
pthread_mutex_t buffer_mutex;

// tamanho do buffer
#define BUFFER_SIZE 16

// inicializa buffer
int buffer[BUFFER_SIZE];

// numero de produtores
#define NUM_PRODUCERS 4

// inicializa array de produtores
pthread_t producers[NUM_PRODUCERS];

// numero de consumidores
#define NUM_CONSUMERS 32

// inicializa array de consumidores
pthread_t consumers[NUM_CONSUMERS];

// cria semaforo de posicoes livres
sem_t free_positions;

// cria semaforo de posicoes ocupadas
sem_t occupied_positions;

// armazena posicao do produtor
int producer_position = 0;

// armazena posicao do consumidor
int consumer_position = 0;

// funcao dos produtores
void *producer(void *arg)
{
	// trata o id do produtor
	int id = *((int *)arg);

	// cria item
	int item;

	// loop infinito
	while (1)
	{
		// item receber inteiro aleatorio de 0 a 99
		item = rand() % 100;

		// semaforo espera sinal para posicoes livre
		sem_wait(&free_positions);

		// da lock no buffer
		pthread_mutex_lock(&buffer_mutex);

		// valor de item eh armazenado em uma posicao do buffer
		buffer[producer_position] = item;

		// escreve na tela qual produtor produziou qual item em qual posicao do buffer
		printf("produtor   [%02d] >> produz  >> item %02d >> posicao %02d\n", id, item, producer_position);

		// incrementa posicao do produtor sem extrapolar o tamanho do buffer
		producer_position = (producer_position + 1) % BUFFER_SIZE;

		// da unlock no buffer
		pthread_mutex_unlock(&buffer_mutex);

		// envia sinal para posicoes ocupadas
		sem_post(&occupied_positions);

		// aguarda 2 segundos
		sleep(2);
	}

	// da exit nos produtores
	pthread_exit(0);
}

// funcao dos consumidores
void *consumer(void *arg)
{
	// trata id dos consumidores
	int id = *((int *)arg);

	// cria item
	int item;

	// loop infinito
	while (1)
	{
		// semaforo espera sinal para posicoes ocupadas
		sem_wait(&occupied_positions);

		// da lock no buffer
		pthread_mutex_lock(&buffer_mutex);

		// item recebe um valor em uma posicao do buffer
		item = buffer[consumer_position];

		// escreve na tela qual consumidor consumiu qual item em qual posicao
		printf("consumidor [%02d] >> consome >> item %02d >> posicao %02d\n", id, item, consumer_position);

		// incrementa posicao do consumidor sem extrapolar o tamanho do buffer
		consumer_position = (consumer_position + 1) % BUFFER_SIZE;

		// da unlock no buffer
		pthread_mutex_unlock(&buffer_mutex);

		// envia sinal para posicoes livres
		sem_post(&free_positions);

		// espera 32 segundos
		sleep(32);
	}

	// da exit nos consumidores
	pthread_exit(0);
}

// main
int main()
{
	// inicializa buffer
	pthread_mutex_init(&buffer_mutex, NULL);

	// inicializa semaforo de posicoes livres
	sem_init(&free_positions, 0, BUFFER_SIZE);

	// inicializa semaforo de posicoes ocupadas
	sem_init(&occupied_positions, 0, 0);

	// cria id
	int *id;

	// loop de criacao dos produtores
	for (int i = 0; i < NUM_PRODUCERS; i++)
	{
		// trata id
		id = (int *)malloc(sizeof(int));
		*id = i;

		// cria thread e verifica por erro
		if (pthread_create(&producers[i], NULL, producer, (void *)(id)))
		{
			// escreve erro na tela
			printf("erro >> criacao produtor >> %02d\n", i);

			// sai do programa
			exit(1);
		}
	}

	// loop de criacao dos consumidores
	for (int i = 0; i < NUM_CONSUMERS; i++)
	{
		// trata id
		id = (int *)malloc(sizeof(int));
		*id = i;

		// cria thread de consumidor e verifica por erro
		if (pthread_create(&consumers[i], NULL, consumer, (void *)(id)))
		{
			// escreve erro na tela
			printf("erro >> criacao consumidor >> %02d\n", i);

			// sai do programa
			exit(1);
		}
	}

	// loop de fechamento das threads dos produtores
	for (int i = 0; i < NUM_PRODUCERS; i++)
	{
		// da join na thread de produtor
		pthread_join(producers[i], NULL);
	}

	// loop de fechamento das threads dos consumidores
	for (int i = 0; i < NUM_CONSUMERS; i++)
	{
		// da join na thread de consumidor
		pthread_join(consumers[i], NULL);
	}

	// retorno da main
	return 0;
}
