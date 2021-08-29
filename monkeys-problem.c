#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define TRUE 1
#define MA 10 // macacos de A para B
#define MB 10 // macacos de B para A

pthread_mutex_t lock_ab;
pthread_mutex_t lock_ba;
pthread_mutex_t lock_rope;
pthread_mutex_t lock_turn;

int ab = 0;
int ba = 0;

void *monkey_ab(void *a)
{
	int i = *((int *)a);
	while (TRUE)
	{
		// acessa a corda
		pthread_mutex_lock(&lock_turn);
		pthread_mutex_lock(&lock_ab);
		ab++;
		if (ab == 1)
		{
			pthread_mutex_lock(&lock_rope);
		}
		pthread_mutex_unlock(&lock_ab);
		pthread_mutex_unlock(&lock_turn);
		printf("MACACO[%02d]  >>  ATRAVESSANDO  >>  A => B\n", i);
		sleep(2);

		// sai da corda
		pthread_mutex_lock(&lock_ab);
		ab--;
		printf("MACACO[%02d]  >>  CHEGOU  >>  B\n", i);
		sleep(1);
		if (ab == 0)
		{
			printf("--\nCORDA LIVRE\n--\n");
			sleep(1);
			pthread_mutex_unlock(&lock_rope);
		}
		pthread_mutex_unlock(&lock_ab);
	}
	pthread_exit(0);
}

void *monkey_ba(void *a)
{
	int i = *((int *)a);
	while (TRUE)
	{
		// acessa a corda
		pthread_mutex_lock(&lock_turn);
		pthread_mutex_lock(&lock_ba);
		ba++;
		if (ba == 1)
		{
			pthread_mutex_lock(&lock_rope);
		}
		pthread_mutex_unlock(&lock_ba);
		pthread_mutex_unlock(&lock_turn);
		printf("MACACO[%02d]  >>  ATRAVESSANDO  >>  B => A\n", i);
		sleep(2);

		// sai da corda
		pthread_mutex_lock(&lock_ba);
		ba--;
		printf("MACACO[%02d]  >>  CHEGOU  >>  A\n", i);
		sleep(1);
		if (ba == 0)
		{
			printf("--\nCORDA LIVRE\n--\n");
			sleep(1);
			pthread_mutex_unlock(&lock_rope);
		}
		pthread_mutex_unlock(&lock_ba);
	}
	pthread_exit(0);
}

void *gorilla_ab(void *a)
{
	while (TRUE)
	{
		//Procedimentos para acessar a corda
		pthread_mutex_lock(&lock_turn);
		pthread_mutex_lock(&lock_rope);
		printf("GORILA  >>  ATRAVESSANDO  >> A => B\n");
		sleep(4);
		printf("GORILA  >>  CHEGOU  >>  B\n");
		sleep(1);
		//Procedimentos para quando sair da corda
		printf("--\nCORDA LIVRE\n--\n");
		sleep(1);
		pthread_mutex_unlock(&lock_rope);
		pthread_mutex_unlock(&lock_turn);
	}
	pthread_exit(0);
}

void *gorilla_ba(void *a)
{
	while (TRUE)
	{
		//Procedimentos para acessar a corda
		pthread_mutex_lock(&lock_turn);
		pthread_mutex_lock(&lock_rope);
		printf("GORILA  >>  ATRAVESSANDO  >>  B => A\n");
		sleep(4);
		printf("GORILA  >>  CHEGOU  >> A\n");
		sleep(1);
		//Procedimentos para quando sair da corda
		printf("--\nCORDA LIVRE\n--\n");
		sleep(1);
		pthread_mutex_unlock(&lock_rope);
		pthread_mutex_unlock(&lock_turn);
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
	pthread_t g_ab, g_ba;
	pthread_create(&g_ab, NULL, &gorilla_ab, NULL);
	pthread_create(&g_ba, NULL, &gorilla_ba, NULL);

	pthread_join(monkeys[0], NULL);
	return 0;
}
