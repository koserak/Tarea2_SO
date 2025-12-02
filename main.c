#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "barrera.h"

#define N 5 // Los threads que se van a sincronizar
#define ETAPAS 4 // Veces que la barra se reutiliza

typedef struct {
    int id_hilo; // Identificadornico thread (0,1,2,...).
    barrera_t *la_barrera; // Puntero a la barrera reutilizable que todos los threads deben usar
} args_hilo_t;

void *trabajador(void *arg){ // Esta función es ejecutada por cada hilo creado en main().
    args_hilo_t *datos = (args_hilo_t*) arg;
    int id = datos->id_hilo;
    barrera_t *barrera = datos->la_barrera;

    for(int i = 0; i < ETAPAS; i++){ // cada thread pasa por la barrera ETAPAS veces
        printf("Hilo %d: Llego a la barrera en la etapa %d\n", id, i);
        waitBarrera(barrera); // Esta barrera sincroniza los hilos, todos llegan aqui antes de seguir
        printf("Hilo %d: Cruzo la barrera en la etapa %d\n", id, i);
    }
    return NULL;
}

int main() {

    pthread_t hilos[N]; // Arreglo de los ids de los threads
    args_hilo_t args[N]; // Arreglo de los argumentos para cada thread

    barrera_t mi_barrera;
    initBarrera(&mi_barrera, N); // Se inicializa la barrera para N threads

    // Aqui se crean los N hilos
    for(int i = 0; i < N; i++){
        args[i].id_hilo = i;
        args[i].la_barrera = &mi_barrera;

        pthread_create(&hilos[i], NULL, trabajador, &args[i]); // Crea cada thread y le pasa los argumentos
    }

    // En este for se espera a los hilos
    for(int i = 0; i < N; i++){
        pthread_join(hilos[i], NULL);
    }

    destruyeBarrera(&mi_barrera); // Se destruyen los recursos de barrera

    return 0;
}


