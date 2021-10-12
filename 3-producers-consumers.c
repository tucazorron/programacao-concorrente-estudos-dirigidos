#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// numero de produtores
#define number_of_producers 1

// numero de consumidores
#define number_of_consumers 1

// capacidade do estoque
#define stock_capacity 5

// cria mutex
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// condicao de produtor
pthread_cond_t producers_conditional = PTHREAD_COND_INITIALIZER;

// condicao de consumidor
pthread_cond_t consumers_conditional = PTHREAD_COND_INITIALIZER;

// contador de produtor
int counter = 0;

// contador de adicao
int adder = 0;

// contador de remocao
int remover = 0;

void *producers_function(void *arg)
{
	int id = *(int *)arg;
	while (1)
	{
		printf("produtor [%02d] >> produz item\n", id);
		sleep(1);

		// da lock
		pthread_mutex_lock(&mutex);

		// enquanto estoque estiver cheio
		while (counter == stock_capacity)
		{
			printf("produtor [%02d] >> espera estoque esvaziar\n", id);
			sleep(1);

			// aguarda consumidores consumirem
			pthread_cond_wait(&producers_conditional, &mutex);
		}

		printf("produtor [%02d] >> adiciona item >> posicao: [%02d]\n", id, adder);
		sleep(1);

		// incrementa adicionador
		adder++;

		// se igualar ou ultrapassar limite, fica zerado
		if (adder >= stock_capacity)
		{
			// zera adicionador
			adder = 0;
		}

		// incrementa contador
		counter++;

		// se tiver apenas um produto disponivel
		if (counter == 1)
		{
			// avisa consumidores que existem novos itens
			pthread_cond_signal(&consumers_conditional);
		}

		// da unlock geral
		pthread_mutex_unlock(&mutex);
	}
	pthread_exit(0);
}

// funcao dos consumidores
void *consumer(void *arg)
{
	int id = *(int *)arg;
	while (1)
	{
		// da lock geral
		pthread_mutex_lock(&mutex);

		// se nao tem produtos disponiveis, consumidor espera
		while (counter == 0)
		{

			// consumidor espera
			printf("consumidor: [%02d] >> espera\n", id);

			// aguarda sinal dos produtores
			pthread_cond_wait(&consumers_conditional, &mutex);
		}

		// consumidor remove item
		printf("consumidor: [%02d] >> remove item   >> posicao: [%02d]\n", id, remover);

		// incrementa removedor
		remover++;

		// se igualar ou ultrapassar o limite, fica zerado
		if (remover >= stock_capacity)
		{
			// zera removedor
			remover = 0;
		}

		// desincrementa contador
		counter--;

		// se nao tiver mais produtos disponiveis, avisa produtor
		if (counter == stock_capacity - 1)
		{
			// avisa produtor para produzir
			pthread_cond_signal(&producers_conditional);
		}

		// da consumers_conditionallock geral
		pthread_mutex_unlock(&mutex);

		// consumidor consome item
		printf("consumidor: [%02d] >> consome item  >> posicao: [%02d]\n", id, remover);
		sleep(10);
	}
	pthread_exit(0);
}

void main(argc, argv)
{
	int i;
	int *id;

	pthread_t producers[number_of_producers], consumers[number_of_consumers];

	for (i = 0; i < number_of_producers; i++)
	{
		id = (int *)malloc(sizeof(int));
		*id = i;
		pthread_create(&producers[i], NULL, producers_function, (void *)(id));
	}

	for (i = 0; i < number_of_consumers; i++)
	{
		id = (int *)malloc(sizeof(int));
		*id = i;
		pthread_create(&consumers[i], NULL, consumer, (void *)(id));
	}

	pthread_join(producers[0], NULL);
}
