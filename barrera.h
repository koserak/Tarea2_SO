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
void destruyeBarrera(barrera_t *B);