#include "barrera.h"

void initBarrera(barrera_t *B, int numT) {
    B->count = 0;
    B->N = numT;
    B->etapa = 0;
    pthread_mutex_init(&B->mutex, NULL);
    pthread_cond_init(&B->cond, NULL);
}

void waitBarrera(barrera_t *B) {
    pthread_mutex_lock(&B->mutex);
    
    // capturar etapa actual en variable local
    int etapaLocal = B->etapa;
    B->count++;
    
    if (B->count < B->N) { //espera otras demas hebras
        while (etapaLocal == B->etapa) {
            pthread_cond_wait(&B->cond, &B->mutex);
        }
    } else { //ultima hebra
        B->etapa++;
        B->count = 0;
        pthread_cond_broadcast(&B->cond);
    }
    
    pthread_mutex_unlock(&B->mutex);
}

void destruyeBarrera(barrera_t *B) {
    pthread_mutex_destroy(&B->mutex);
    pthread_cond_destroy(&B->cond);
}