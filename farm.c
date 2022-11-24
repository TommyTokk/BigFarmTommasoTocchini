#include "utils.h"
#include "worker.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#define QUI __LINE__,__FILE__

// parametri del server
#define HOST "127.0.0.1"
#define PORT 65432

/*
Programma (multi-thread) che che prende dalla linea di comando
una lista di file binari e una serie di parametri.

TO DO:
1) Creazione corpo della funzione Worker
  1.1) Sistemare logica per quanto riguarda il nome del file
  1.2) Sistemare eventuali errori nel worker
  1.3) Creazione della socket per la comunicazione
2) Creazione corpo della funzione MasterWorker 
3) Gestire il comando SIGINT
*/





int main(int argc, char *argv[]){
  // controlla numero argomenti
  if(argc<2) {
      printf("Uso: %s file [file ...] \n",argv[0]);
      return 1;
  }

  int nThreads, buffSize, delay, opt = 0;

  while ((opt = getopt(argc, argv, "n:q:t:")) != -1){
    switch (opt){
    case 'n':
      nThreads = atoi(optarg) < 1 ? 4: atoi(optarg);
      break;
    case 'q':
      buffSize = atoi(optarg) <= 0 ? 8: atoi(optarg);
      break;
    case 't':
      delay = atoi(optarg) < 0 ? 0: atoi(optarg);
      break;
    }
  }

  pthread_t *threads = malloc(nThreads * sizeof(pthread_t));
  if(!threads) termina("Allocazione memoria thread fallito");

  char **buff = malloc(buffSize * sizeof(char *));
  if(!buff) termina ("Allocazione buffer fallita");
  //Dichiarazione semafori 
  sem_t sem_free_slots, sem_data_access;
  xsem_init(&sem_free_slots,0,buffSize,__LINE__,__FILE__);
  xsem_init(&sem_data_access,0,0,__LINE__,__FILE__);


  pthread_mutex_t cMutex = PTHREAD_MUTEX_INITIALIZER;

  tData d;
  d.cIndex = 0;
  d.buffer = buff;
  d.buffSize = buffSize;
  d.cMutex = &cMutex;
  d.fileName = 266;
  d.sem_free_slots = &sem_free_slots;
  d.sem_data_access = &sem_data_access;

  for(int i = 0; i < nThreads; i++){
    xpthread_create(&threads[i],NULL,&WorkerBody,&d,QUI);
  }
  
  free(buff);
  free(threads);
	return 0;
}