#include "stdio.h"
#include "unistd.h"
#include "stdlib.h"
#include "pthread.h"

#define TRUE 1

#define NE 3  //numero de escritores
#define NL 10 //numero de leitores

pthread_mutex_t lock_bd = PTHREAD_MUTEX_INITIALIZER;	// mutex do bd
pthread_mutex_t lock_nl = PTHREAD_MUTEX_INITIALIZER;	// mutex dos leitores
pthread_mutex_t lock_turns = PTHREAD_MUTEX_INITIALIZER; // mutex dos turnos

int num_leitores = 0;

void *reader(void *arg);
void *writer(void *arg);
void read_data_base();
void use_data_read();
void think_up_data();
void write_data_base();

int main()
{
	pthread_t r[NL], w[NE];
	int i;
	int *id;

	// cria leitores
	for (i = 0; i < NL; i++)
	{
		id = (int *)malloc(sizeof(int));
		*id = i;
		pthread_create(&r[i], NULL, reader, (void *)(id));
	}

	// cria escritores
	for (i = 0; i < NE; i++)
	{
		id = (int *)malloc(sizeof(int));
		*id = i;
		pthread_create(&w[i], NULL, writer, (void *)(id));
	}
	pthread_join(r[0], NULL);
	return 0;
}

void *reader(void *arg)
{
	int i = *((int *)arg);
	while (TRUE)
	{
		pthread_mutex_lock(&lock_turns); // da lock nos turnos
		pthread_mutex_lock(&lock_nl);	 // da lock nos leitores
		num_leitores++;					 // incrementa o numero de leitores
		if (num_leitores == 1)			 // se tiver apenas um leitor acessando, da lock no bd
		{
			pthread_mutex_lock(&lock_bd); // da lock no bd
		}
		pthread_mutex_unlock(&lock_nl);	   // da unlock nos leitores
		pthread_mutex_unlock(&lock_turns); // da unlock nos turnos

		read_data_base(i); // acessa os dados

		pthread_mutex_lock(&lock_nl); // da lock nos leitores
		num_leitores--;				  // diminui o numero de leitores
		if (num_leitores == 0)		  // se nao tiver nenhum leitor acessando o bd, da unlock no bd
		{
			pthread_mutex_unlock(&lock_bd); // da unlock no bd
		}
		pthread_mutex_unlock(&lock_nl); // da unlock nos leitores
		use_data_read(i);				// usa os dados lidos (regiao nao critica)
	}
	pthread_exit(0);
}

void *writer(void *arg)
{
	int i = *((int *)arg);
	while (TRUE)
	{
		think_up_data(i);				   // pensa no que escrever (regiao nao critica)
		pthread_mutex_lock(&lock_turns);   // da lock nos turnos
		pthread_mutex_lock(&lock_bd);	   // da lock no bd
		write_data_base(i);				   // escreve no bd
		pthread_mutex_unlock(&lock_bd);	   // da unlock no bd
		pthread_mutex_unlock(&lock_turns); // da unlock nos turnos
	}
	pthread_exit(0);
}

void read_data_base(int i)
{
	printf("LEITOR[%d] >> LENDO\n", i);
	sleep(2);
}

void use_data_read(int i)
{
	printf("LEITOR[%d] >> USANDO\n", i);
	sleep(1);
}

void think_up_data(int i)
{
	printf("ESCRITOR[%d] >> PENSANDO\n", i);
	sleep(1);
}

void write_data_base(int i)
{
	printf("ESCRITOR[%d] >> ESCREVENDO\n", i);
	sleep(4);
}
