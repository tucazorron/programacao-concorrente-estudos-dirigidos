#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MAXCANIBAIS 20

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;       // mutex geral
pthread_cond_t cond_cooker = PTHREAD_COND_INITIALIZER;   // condicao de cozinheiro
pthread_cond_t cond_cannibal = PTHREAD_COND_INITIALIZER; // condicao de canibal

int food = 0; // comida

void *cannibal(void *id);
void *cooker(int portions);

void main(argc, argv) int argc;
char *argv[];
{
  int erro;
  int i;
  int cannibals;
  int portions;
  int *id;

  pthread_t tid[MAXCANIBAIS];

  if (argc != 3)
  {
    printf("erro na chamada do programa: jantar <#canibais> <#comida>\n");
    exit(1);
  }

  cannibals = atoi(argv[1]); // numero de canibais
  portions = atoi(argv[2]);  // quantidade de porções que o cooker consegue preparar por vez
  printf("numero de canibais: %d\nquantidade de comida: %d\n----\n", cannibals, portions);

  if (cannibals > MAXCANIBAIS)
  {
    printf("o numero de canibais e' maior que o maximo permitido: %d\n", MAXCANIBAIS);
    exit(1);
  }

  for (i = 0; i < cannibals; i++)
  {
    id = (int *)malloc(sizeof(int));
    *id = i;
    erro = pthread_create(&tid[i], NULL, cannibal, (void *)(id));

    if (erro)
    {
      printf("erro na criacao do thread %d\n", i);
      exit(1);
    }
  }

  cooker(portions);
}

void *cannibal(void *pi)
{

  while (1)
  {
    pthread_mutex_lock(&mutex); // da lock nas threads

    while (food == 0) // se nao tiver comida, canibal espera
    {
      printf("canibal: %d | espera comida     |\n", *(int *)(pi)); // canibal espera
      pthread_cond_wait(&cond_cannibal, &mutex);                   // espera sinal
    }

    printf("canibal: %d | serve comida      | porcoes: %d\n", *(int *)(pi), food); // canibal serve comida

    food--;        // comida diminui
    if (food == 0) // se acabar a comida, canibal avorda cozinheiro
    {
      printf("canibal: %d | acorda cozinheiro |\n", *(int *)(pi)); // canibal acorda cozinheiro
      pthread_cond_signal(&cond_cooker);
    }

    pthread_mutex_unlock(&mutex); // da unlock nas threads

    printf("canibal: %d | come comida       |\n", *(int *)(pi)); // canibal come
    sleep(4);
  }
}

void *cooker(int portions)
{

  while (1)
  {

    pthread_mutex_lock(&mutex); // da lock nas threads
    while (food != 0)           // se tiver comida, cozinheiro aguarda
    {
      printf("cozinheiro | aguarda           | porcoes: %d\n", food); // cozinheiro aguarda
      pthread_cond_wait(&cond_cooker, &mutex);                        // espera sinal para continuar
    }

    printf("cozinheiro | cozinha           |\n"); // cozinheiro cozinha
    sleep(4);

    food += portions; // aumenta a comida com mais porcoes

    pthread_cond_broadcast(&cond_cannibal); // avisa os canibais que tem comida
    pthread_mutex_unlock(&mutex);           // da unlock nas threads
  }
}
