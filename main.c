#include <pthread.h>
#include "barrera.h"
#include <stdio.h>
#include <unistd.h>

#define N 5
#define ETAPAS 4

typedef struct {
    int id_hilo;
    barrera_t *la_barrera;
} args_hilo_t;


void *trabajador(void *arg){
	args_hilo_t *datos = (args_hilo_t*) arg;
	int id = datos->id_hilo;
	barrera_t *barrera = datos->la_barrera;
	printf("Soy el hilo %d y voy a esperar...\n", id);
	for(int i=0, i < N, i++){

	}
	return NULL;
}

int main(int argc, char const *argv[])
{
	

	return 0;
}

/*
#include <pthread.h>

typedef struct Barrera_t
{
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int count;
    int N;
    int etapa;
} barrera_t;

void initBarrera(barrera_t *B,int numT);
void waitBarrera(barrera_t *B);
void destruyeBarrera(barrera_t *B);*/