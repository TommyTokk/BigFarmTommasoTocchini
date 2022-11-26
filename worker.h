//Utils functions
#include "utils.h"
typedef struct{
    //long sommaTot;
    int *cIndex;
    char **buffer;
    int *buffSize;
    int fileName;
    pthread_mutex_t *cMutex;
    sem_t *sem_free_slots;
    sem_t *sem_data_access;
}Data;

void *WorkerBody(void *args);
