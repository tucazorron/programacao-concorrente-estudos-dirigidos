#include "pthread.h"
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"

// numero de escritores
#define number_of_writers 3

// numero de leitores
#define number_of_readers 10

// mutex do banco de dados
pthread_mutex_t database_lock = PTHREAD_MUTEX_INITIALIZER;

// mutex dos leitores
pthread_mutex_t readers_lock = PTHREAD_MUTEX_INITIALIZER;

// mutex dos turnos
pthread_mutex_t turns_lock = PTHREAD_MUTEX_INITIALIZER;

// numero de leitores no banco de dados
int database_readers = 0;

// le o banco de dados
void reads_database(int id)
{
	printf("leitor [%02d] >> le o banco de dados\n", id);
	sleep(1);
}

// usa o dado lido
void uses_read_data(int id)
{
	printf("leitor [%02d] >> usa os dados\n", id);
	sleep(1);
}

// pensa no que escrever
void thinks_about_what_to_write(int id)
{
	printf("escritor [%02d] >> pensa no que escrever\n", id);
	sleep(1);
}

// escreve no banco de dados
void writes_in_database(int id)
{
	printf("escritor [%02d] >> escreve no banco de dados\n", id);
	sleep(1);
}

// funcao dos leitores
void *readers_function(void *arg)
{
	int id = *((int *)arg);
	while (1)
	{
		// da lock nos turnos
		pthread_mutex_lock(&turns_lock);

		// da lock nos leitores
		pthread_mutex_lock(&readers_lock);

		// incrementa o numero de leitores
		database_readers++;

		// se tiver apenas um leitor acessando, da lock no bd
		if (database_readers == 1)
		{
			// da lock no bd
			pthread_mutex_lock(&database_lock);
		}
		// da unlock nos leitores
		pthread_mutex_unlock(&readers_lock);

		// da unlock nos turnos
		pthread_mutex_unlock(&turns_lock);

		// acessa os dados
		reads_database(id);

		// da lock nos leitores
		pthread_mutex_lock(&readers_lock);

		// diminui o numero de leitores
		database_readers--;

		// se nao tiver nenhum leitor acessando o bd, da unlock no bd
		if (database_readers == 0)
		{
			// da unlock no bd
			pthread_mutex_unlock(&database_lock);
		}

		// da unlock nos leitores
		pthread_mutex_unlock(&readers_lock);

		// usa os dados lidos (regiao nao critica)
		uses_read_data(id);
	}
	pthread_exit(0);
}

// funcao dos escritores
void *writers_function(void *arg)
{
	int id = *((int *)arg);
	while (1)
	{
		// pensa no que escrever (regiao nao critica)
		thinks_about_what_to_write(id);

		// da lock nos turnos
		pthread_mutex_lock(&turns_lock);

		// da lock no bd
		pthread_mutex_lock(&database_lock);

		// escreve no bd
		writes_in_database(id);

		// da unlock no bd
		pthread_mutex_unlock(&database_lock);

		// da unlock nos turnos
		pthread_mutex_unlock(&turns_lock);
	}
	pthread_exit(0);
}

int main()
{
	pthread_t readers[number_of_readers], writers[number_of_writers];
	int i;
	int *id;

	// cria leitores
	for (i = 0; i < number_of_readers; i++)
	{
		id = (int *)malloc(sizeof(int));
		*id = i;
		pthread_create(&readers[i], NULL, readers_function, (void *)(id));
	}

	// cria escritores
	for (i = 0; i < number_of_writers; i++)
	{
		id = (int *)malloc(sizeof(int));
		*id = i;
		pthread_create(&writers[i], NULL, writers_function, (void *)(id));
	}

	pthread_join(readers[0], NULL);
	return 0;
}
