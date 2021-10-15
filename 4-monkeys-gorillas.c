#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// numero de macacos
#define number_of_monkeys

// numero de macacos de a para b
#define number_of_monkeys_from_a_to_b 10

// numero de macacos de b para a
#define number_of_monkeys_from_b_to_a 10

// mutex de a para b
pthread_mutex_t a_to_b;

// mutex de b para a
pthread_mutex_t b_to_a;

// mutex da corda
pthread_mutex_t rope;

// mutex dos turnos
pthread_mutex_t turns;

// macacos atravessando de a para b
int rope_a_to_b = 0;

// macacos atravessando de b para a
int rope_b_to_a = 0;

// funcao dos macacos de a para b
void *monkeys_a_to_b_function(void *arg)
{
	int id = *(int *)arg;

	while (1)
	{
		// da lock no turno
		pthread_mutex_lock(&turns);

		// da lock de a para b
		pthread_mutex_lock(&a_to_b);

		// incrementa o numero de macacos de a para b
		rope_a_to_b++;

		// se for o primeiro macaco, da lock na corda
		if (rope_a_to_b == 1)

		{
			// da lock na corda
			pthread_mutex_lock(&rope);
		}

		// da unlock de a para b
		pthread_mutex_unlock(&a_to_b);

		// da unlock no turno
		pthread_mutex_unlock(&turns);

		printf("macaco [%02d]  >>  atravessando  >>  a => b\n", id);
		sleep(1);

		printf("macaco [%02d]  >>  chegou        >>  b\n", id);
		sleep(1);

		// da lock de a para b
		pthread_mutex_lock(&a_to_b);

		// diminui o numero de macacos atravessando de a para b
		rope_a_to_b--;

		// se nao tiver mais macacos atravessando de a para b, libera a corda
		if (rope_a_to_b == 0)
		{
			printf("--\ncorda livre\n--\n");
			sleep(1);

			// da unlock na corda
			pthread_mutex_unlock(&rope);
		}

		// da unlock de a para b
		pthread_mutex_unlock(&a_to_b);
	}
	pthread_exit(0);
}

void *monkeys_b_to_a_function(void *arg)
{
	int id = *(int *)arg;

	while (1)
	{
		// da lock no turno
		pthread_mutex_lock(&turns);

		// da lock de b para a
		pthread_mutex_lock(&b_to_a);

		// incrementa o numero de macacos de b para a
		rope_b_to_a++;

		// se for o primeiro macaco, da lock na corda
		if (rope_b_to_a == 1)
		{

			// da lock na corda
			pthread_mutex_lock(&rope);
		}

		// da unlock de b para a
		pthread_mutex_unlock(&b_to_a);

		// da unlock no turno
		pthread_mutex_unlock(&turns);

		printf("macaco [%02d]  >>  atravessando  >>  B => A\n", id);
		sleep(1);

		printf("macaco [%02d]  >>  chegou        >>  A\n", id);
		sleep(1);

		// da lock de b para a
		pthread_mutex_lock(&b_to_a);

		// diminui o numero de macacos atravessando de b para a
		rope_b_to_a--;

		// se nao tiver mais macacos atravessando de b para a, libera a corda
		if (rope_b_to_a == 0)
		{
			printf("--\nCORDA LIVRE\n--\n");
			sleep(1);

			// da unlock na corda
			pthread_mutex_unlock(&rope);
		}

		// da unlock de b para a
		pthread_mutex_unlock(&b_to_a);
	}
	pthread_exit(0);
}

void *gorillas_a_to_b_function(void *arg)
{
	while (1)
	{

		// da lock no turno
		pthread_mutex_lock(&turns);

		// da lock na corda
		pthread_mutex_lock(&rope);

		// escreve na tela que gorila esta atravessando de a para b
		printf("GORILA      >>  atravessando  >>  A => B\n");

		// trava a tela por 4 segundos
		sleep(1);

		// escreve na tela que gorila chegou em b
		printf("GORILA      >>  chegou        >>  B\n");

		// trava a tela por 1 segundo
		sleep(1);

		// escreve na tela que a corda esta livre
		printf("--\nCORDA LIVRE\n--\n");

		// trava a tela por 1 segundo
		sleep(1);

		// da unlock na corda
		pthread_mutex_unlock(&rope);

		// da unlock no turno
		pthread_mutex_unlock(&turns);
	}
	pthread_exit(0);
}

void *gorillas_b_to_a_function(void *arg)
{

	// loop infinito
	while (1)
	{

		// da lock no turno
		pthread_mutex_lock(&turns);

		// da lock na corda
		pthread_mutex_lock(&rope);

		// escreve na tela que gorila esta atravessando de b para a
		printf("GORILA      >>  atravessando  >>  B => A\n");

		// trava a tela por 4 segundos
		sleep(1);

		// escreve na tela que gorila chegou em a
		printf("GORILA      >>  chegou        >>  A\n");

		// trava a tela por 1 segundo
		sleep(1);

		// escreve na tela que a corda esta livre
		printf("--\nCORDA LIVRE\n--\n");

		// trava a tela por 1 segundo
		sleep(1);

		// da unlock na corda
		pthread_mutex_unlock(&rope);

		// da unlock no turno
		pthread_mutex_unlock(&turns);
	}
	pthread_exit(0);
}

int main(int argc, char *argv[])
{
	pthread_t monkeys[number_of_monkeys_from_a_to_b + number_of_monkeys_from_b_to_a];
	int *id;
	int i = 0;

	for (i = 0; i < number_of_monkeys_from_a_to_b + number_of_monkeys_from_b_to_a; i++)
	{
		id = (int *)malloc(sizeof(int));
		*id = i;
		if (i % 2 == 0)
		{
			pthread_create(&monkeys[i], NULL, &monkeys_a_to_b_function, (void *)id);
		}
		else
		{
			pthread_create(&monkeys[i], NULL, &monkeys_b_to_a_function, (void *)id);
		}
	}

	pthread_t gorillas[2];
	pthread_create(&gorillas[0], NULL, &gorillas_a_to_b_function, NULL);
	pthread_create(&gorillas[1], NULL, &gorillas_b_to_a_function, NULL);

	pthread_join(monkeys[0], NULL);
	return 0;
}
