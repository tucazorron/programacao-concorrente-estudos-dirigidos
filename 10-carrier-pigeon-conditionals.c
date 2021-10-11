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

// cria mutex
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

// cria condicional de pombo
pthread_cond_t pigeon_conditional = PTHREAD_COND_INITIALIZER;

// cria condicional de usuarios
pthread_cond_t users_conditional = PTHREAD_COND_INITIALIZER;

// funcao do pombo
void *pigeon_function(void *arg)
{
	int i = 0;
	while (1)
	{
		// da lock no mutex
		pthread_mutex_lock(&lock);

		// enquanto o numero de cartas na mochila for menor que a capacidade total
		while (letters < bag_capacity)
		{
			// pombo espera liberacao para viajar
			pthread_cond_wait(&pigeon_conditional, &lock);
		}

		// mochila esta cheia e o pombo entrega as cartas
		printf("\npombo  >> mochila cheia >> entrega cartas\n");
		sleep(1);

		// pombo entregou todas as cartas e voltou
		printf("pombo  >> mochila vazia >> voltou\n\n");
		sleep(1);

		// zera numero de cartas na mochila
		letters = 0;

		// avisa os usuarios para voltarem a escrever
		pthread_cond_broadcast(&users_conditional);

		// da unlock no mutex
		pthread_mutex_unlock(&lock);
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

		// da lock no mutex
		pthread_mutex_lock(&lock);

		while (letters == bag_capacity)
		{
			pthread_cond_wait(&users_conditional, &lock);
		}

		// incrementa o numero da carta
		letters++;

		// coloca carta na mochila
		printf("usuario [%02d] >> coloca carta na mochila >> carta [%02d]\n", id, letters);
		sleep(1);

		// verifica se a mochila esta cheia
		if (letters == bag_capacity)
		{
			// avisa o pombo para enviar cartas
			pthread_cond_signal(&pigeon_conditional);
		}

		// da unlock
		pthread_mutex_unlock(&lock);
	}
}

int main(int argc, char **argv)
{
	int i;

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
