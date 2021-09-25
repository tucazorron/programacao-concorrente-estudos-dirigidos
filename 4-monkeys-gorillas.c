#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

// numero de macacos de a para b
#define MA 10

// numero de macacos de b para a
#define MB 10

pthread_mutex_t lock_ab;   // mutex de a para b
pthread_mutex_t lock_ba;   // mutex de b para a
pthread_mutex_t lock_rope; // mutex da corda
pthread_mutex_t lock_turn; // mutex dos turnos

int ab = 0; // macacos atravessando de a para b
int ba = 0; // macacos atravessando de b para a

void *monkey_ab(void *a)
{
	int i = *((int *)a); // recebe id do macaco

	while (1) // loop infinito
	{
		pthread_mutex_lock(&lock_turn); // da lock no turno
		pthread_mutex_lock(&lock_ab);	// da lock de a para b

		ab++;		 // incrementa o numero de macacos de a para b
		if (ab == 1) // se for o primeiro macaco, da lock na corda
		{
			pthread_mutex_lock(&lock_rope); // da lock na corda
		}

		pthread_mutex_unlock(&lock_ab);	  // da unlock de a para b
		pthread_mutex_unlock(&lock_turn); // da unlock no turno

		printf("MACACO[%02d]  |  ATRAVESSANDO  |  A => B\n", i); // escreve na tela qual macaco esta atravessando de a para b
		sleep(2);												 // trava a tela por 2 segundos

		printf("MACACO[%02d]  |  CHEGOU        |  B\n", i); // escreve na tela qual macaco chegou em b
		sleep(1);											// trava a tela por 1 segundo

		pthread_mutex_lock(&lock_ab); // da lock de a para b

		ab--;		 // diminui o numero de macacos atravessando de a para b
		if (ab == 0) // se nao tiver mais macacos atravessando de a para b, libera a corda
		{
			printf("--\nCORDA LIVRE\n--\n"); // escreve na tela que a corda esta livre
			sleep(1);						 // trava a tela por 1 segundo

			pthread_mutex_unlock(&lock_rope); // da unlock na corda
		}
		pthread_mutex_unlock(&lock_ab); // da unlock de a para b
	}
	pthread_exit(0);
}

void *monkey_ba(void *a)
{
	int i = *((int *)a); // recebe id do macaco

	while (1) // loop infinito
	{
		pthread_mutex_lock(&lock_turn); // da lock no turno
		pthread_mutex_lock(&lock_ba);	// da lock de b para a

		ba++;		 // incrementa o numero de macacos de b para a
		if (ba == 1) // se for o primeiro macaco, da lock na corda
		{
			pthread_mutex_lock(&lock_rope); // da lock na corda
		}

		pthread_mutex_unlock(&lock_ba);	  // da unlock de b para a
		pthread_mutex_unlock(&lock_turn); // da unlock no turno

		printf("MACACO[%02d]  |  ATRAVESSANDO  |  B => A\n", i); // escreve na tela qual macaco esta atravessando de b para a
		sleep(2);												 // trava a tela por 2 segundos

		printf("MACACO[%02d]  |  CHEGOU        |  A\n", i); // escreve na tela qual macaco chegou em a
		sleep(1);											// trava a tela por 1 segundo

		pthread_mutex_lock(&lock_ba); // da lock de b para a

		ba--;		 // diminui o numero de macacos atravessando de b para a
		if (ba == 0) // se nao tiver mais macacos atravessando de b para a, libera a corda
		{
			printf("--\nCORDA LIVRE\n--\n"); // escreve na tela que a corda esta livre
			sleep(1);						 // trava a tela por 1 segundo

			pthread_mutex_unlock(&lock_rope); // da unlock na corda
		}
		pthread_mutex_unlock(&lock_ba); // da unlock de b para a
	}
	pthread_exit(0);
}

void *gorilla_ab(void *a)
{
	while (1) // loop infinito
	{
		pthread_mutex_lock(&lock_turn); // da lock no turno
		pthread_mutex_lock(&lock_rope); // da lock na corda

		printf("GORILA      |  ATRAVESSANDO  |  A => B\n"); // escreve na tela que gorila esta atravessando de a para b
		sleep(4);											// trava a tela por 4 segundos

		printf("GORILA      |  CHEGOU        |  B\n"); // escreve na tela que gorila chegou em b
		sleep(1);									   // trava a tela por 1 segundo

		printf("--\nCORDA LIVRE\n--\n"); // escreve na tela que a corda esta livre
		sleep(1);						 // trava a tela por 1 segundo

		pthread_mutex_unlock(&lock_rope); // da unlock na corda
		pthread_mutex_unlock(&lock_turn); // da unlock no turno
	}
	pthread_exit(0);
}

void *gorilla_ba(void *a)
{
	while (1) // loop infinito
	{
		pthread_mutex_lock(&lock_turn); // da lock no turno
		pthread_mutex_lock(&lock_rope); // da lock na corda

		printf("GORILA      |  ATRAVESSANDO  |  B => A\n"); // escreve na tela que gorila esta atravessando de b para a
		sleep(4);											// trava a tela por 4 segundos

		printf("GORILA      |  CHEGOU        |  A\n"); // escreve na tela que gorila chegou em a
		sleep(1);									   // trava a tela por 1 segundo

		printf("--\nCORDA LIVRE\n--\n"); // escreve na tela que a corda esta livre
		sleep(1);						 // trava a tela por 1 segundo

		pthread_mutex_unlock(&lock_rope); // da unlock na corda
		pthread_mutex_unlock(&lock_turn); // da unlock no turno
	}
	pthread_exit(0);
}

int main(int argc, char *argv[])
{
	pthread_t monkeys[MA + MB];
	int *id;
	int i = 0;

	for (i = 0; i < MA + MB; i++)
	{
		id = (int *)malloc(sizeof(int));
		*id = i;
		if (i % 2 == 0)
		{
			if (pthread_create(&monkeys[i], NULL, &monkey_ab, (void *)id))
			{
				printf("Não pode criar a thread %d\n", i);
				return -1;
			}
		}
		else
		{
			if (pthread_create(&monkeys[i], NULL, &monkey_ba, (void *)id))
			{
				printf("Não pode criar a thread %d\n", i);
				return -1;
			}
		}
	}

	pthread_t gorillas[2];
	pthread_create(&gorillas[0], NULL, &gorilla_ab, NULL);
	pthread_create(&gorillas[1], NULL, &gorilla_ba, NULL);

	pthread_join(monkeys[0], NULL);
	return 0;
}
