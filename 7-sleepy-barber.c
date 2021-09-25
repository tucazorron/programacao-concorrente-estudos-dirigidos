#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

// numero de clientes
#define clients_quantity 50

// numero de cadeira
#define chairs_quantity 5

// semaforo para cadeiras de espera
sem_t waiting_chair;

// semaforo para cliente novo
sem_t new_client;

// semaforo para cadeira do barbeiro
sem_t barber_chair;

// semaforo para cortar cabelo
sem_t haircut;

// funcao do barbeiro
void *barber_function(void *v)
{
  // loop infinito
  while (1)
  {
    // espera sinal para acordar barbeiro
    sem_wait(&new_client);

    // corta o cabelo
    printf("+ barbeiro >> corta cabelo\n");
    sleep(1);

    // libera cliente apos cortar cabelo
    sem_post(&haircut);
  }
  pthread_exit(0);
}

// funcao do cliente
void *client_function(void *v)
{
  int id = *(int *)v;
  sleep(id % 3);

  // verifica se conseguiu pegar uma cadeira de espera
  if (sem_trywait(&waiting_chair) == 0)
  {
    // cliente entra na barbearia
    printf("cliente %02d >> entra na barbearia\n", id);

    // espera cadeira do barbeiro ficar vaga
    sem_wait(&barber_chair);

    // senta na cadeira do barbeiro
    printf("cliente %02d >> senta na cadeira do barbeiro\n", id);

    // libera cadeira de espera
    sem_post(&waiting_chair);

    // acorda barbeiro
    sem_post(&new_client);

    // espera seu seu cabelo ser cortado
    sem_wait(&haircut);

    // corta o cabelo
    printf("cliente %02d >> corta o cabelo\n", id);

    // libera cadeira do barbeiro
    sem_post(&barber_chair);
  }
  else
  {
    // encontra barbearia cheia
    printf("cliente %02d >> encontra barbearia cheia\n", id);
  }

  // sai da barbearia
  printf("cliente %02d >> sai da barbearia\n", id);
  
  pthread_exit(0);
}

int main()
{
  // cria threads dos clientes e do barbeiro
  pthread_t clients[clients_quantity], barber;
  int i, id[clients_quantity];

  // inicia semaforo de cadeiras de espera
  sem_init(&waiting_chair, 0, chairs_quantity);

  // inicia semaforo de cliente novo
  sem_init(&new_client, 0, 0);

  // inicia semaforo de cadeira do barbeiro
  sem_init(&barber_chair, 0, 1);

  // inicia semaforo de corte de cabelo
  sem_init(&haircut, 0, 0);
  
  // cria clientes
  for (i = 0; i < clients_quantity; i++)
  {
    id[i] = i;
    pthread_create(&clients[i], NULL, client_function, (void *)&id[i]);
  }

  // cria barbeiro
  pthread_create(&barber, NULL, barber_function, NULL);
  
  // da join nas threads dos clientes
  for (i = 0; i < clients_quantity; i++)
    pthread_join(clients[i], NULL);
  return 0;
}
