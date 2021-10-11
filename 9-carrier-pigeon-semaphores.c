#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// inicializa numero de usuarios
#define number_of_users 10

// inicializa capacidade da mochila
#define bag_capacity 20

// inicializa numero de cartas
int letters = 0;

// inicializa semaforo de mochila cheia
sem_t full_bag;

// inicializa semaforo de mochila sendo preenchida
sem_t loading_bag;

// inicializa semaforo de mutex
sem_t lock;

// funcao do pombo
void *pigeon_function(void *arg)
{
	int i = 0;
	while (1)
	{
		// espera encher a mochila
		sem_wait(&full_bag);

		// mochila esta cheia e o pombo entrega as cartas
		printf("\npombo  >> mochila cheia >> entrega cartas\n");
		sleep(1);

		// pombo entregou todas as cartas e voltou
		printf("pombo  >> mochila vazia >> voltou\n\n");
		sleep(1);

		// zera numero de cartas na mochila
		letters = 0;

		// avisa os usuarios que podem voltar a colocar cartas na mochila
		for (i = 0; i < bag_capacity; i++)
		{
			// avisa usuario para abastecer a mochila
			sem_post(&loading_bag);
		}
	}
}

// funcao dos usuarios
void *users_function(void *arg)
{
	// pega o id
	int id = *((int *)arg);
	while (1)
	{
		// escreve carta
		sleep(1);

		// espera a mochila esvaziar
		sem_wait(&loading_bag);

		// da lock com semaforo
		sem_wait(&lock);

		// incrementa o numero da carta
		letters++;

		// coloca carta na mochila
		printf("usuario [%02d] >> coloca carta na mochila >> carta [%02d]\n", id, letters);
		sleep(1);

		// verifica se a mochila esta cheia
		if (letters == bag_capacity)
		{
			// avisa o pombo para enviar cartas
			sem_post(&full_bag);
		}

		// da unlock com semaforo
		sem_post(&lock);
	}
}

int main(int argc, char **argv)
{
	int i;

	// cria semaforos
	sem_init(&full_bag, 0, 0);
	sem_init(&loading_bag, 0, bag_capacity);
	sem_init(&lock, 0, 1);

	// cria threads dos usuarios
	pthread_t users[number_of_users];
	int *id;
	for (i = 0; i < number_of_users; i++)
	{
		id = (int *)malloc(sizeof(int));
		*id = i;
		pthread_create(&(users[i]), NULL, users_function, (void *)(id));
	}

	// cria thread do pombo
	pthread_t pombo;
	id = (int *)malloc(sizeof(int));
	*id = 0;
	pthread_create(&(pombo), NULL, pigeon_function, (void *)(id));

	pthread_join(pombo, NULL);
}
