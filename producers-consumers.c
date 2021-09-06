#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define PR 1
#define CN 1
#define N 5

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;       // mutex geral
pthread_cond_t cond_producer = PTHREAD_COND_INITIALIZER; // condicao de produtor
pthread_cond_t cond_consumer = PTHREAD_COND_INITIALIZER; // condicao de consumidor

int counter = 0; // contador de produtor
int adder = 0;   // contador de adicao
int remover = 0; // contador de remocao

void *producer(void *id);
void *consumer(void *id);
void main(argc, argv) int argc;
char *argv[];
{

  int erro;
  int i, n, m;
  int *id;

  pthread_t tid[PR];

  for (i = 0; i < PR; i++)
  {
    id = (int *)malloc(sizeof(int));
    *id = i;
    erro = pthread_create(&tid[i], NULL, producer, (void *)(id));

    if (erro)
    {
      printf("erro na criacao do thread %d\n", i);
      exit(1);
    }
  }

  pthread_t tCid[CN];

  for (i = 0; i < CN; i++)
  {
    id = (int *)malloc(sizeof(int));
    *id = i;
    erro = pthread_create(&tCid[i], NULL, consumer, (void *)(id));

    if (erro)
    {
      printf("erro na criacao do thread %d\n", i);
      exit(1);
    }
  }

  pthread_join(tid[0], NULL);
}

void *producer(void *id)
{
  while (1)
  {
    printf("produtor:   %d | produz item\n", *(int *)(id)); // produtor produz
    sleep(1);

    pthread_mutex_lock(&mutex); // da lock geral

    while (counter == N) // se pratileira estiver cheia, aguarda diminuir
    {
      printf("produtor:   %d | espera\n", *(int *)(id)); // produtor espera
      pthread_cond_wait(&cond_producer, &mutex);         // da um sinal para produtor aguardar
    }

    printf("produtor:   %d | adiciona item | posicao: %d\n", *(int *)(id), adder); // produtor adiciona

    adder++;        // incrementa adicionador
    if (adder >= N) // se igualar ou ultrapassar limite, fica zerado
    {
      adder = 0; // zera adicionador
    }

    counter++;        // incrementa contador
    if (counter == 1) // se tiver apenas um produto disponivel
    {
      pthread_cond_signal(&cond_consumer); // avisa consumidores que existem novos itens
    }

    pthread_mutex_unlock(&mutex); // da unlock geral
  }
  pthread_exit(0);
}

void *consumer(void *id)
{
  while (1)
  {
    pthread_mutex_lock(&mutex); // da lock geral

    while (counter == 0) // se nao tem produtos disponiveis, consumidor espera
    {
      printf("consumidor: %d | espera\n", *(int *)(id)); // consumidor espera
      pthread_cond_wait(&cond_consumer, &mutex);         // aguarda sinal dos produtores
    }

    printf("consumidor: %d | remove item   | posicao: %d\n", *(int *)(id), remover); // consumidor remove item

    remover++;        // incrementa removedor
    if (remover >= N) // se igualar ou ultrapassar o limite, fica zerado
    {
      remover = 0; // zera removedor
    }

    counter--;            // desincrementa contador
    if (counter == N - 1) // se nao tiver mais produtos disponiveis, avisa produtor
    {
      pthread_cond_signal(&cond_producer); // avisa produtor para produzir
    }

    pthread_mutex_unlock(&mutex); // da unlock geral

    printf("consumidor: %d | consome item  | posicao: %d\n", *(int *)(id), remover); // consumidor consome item
    sleep(10);
  }
  pthread_exit(0);
}
