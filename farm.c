#include "utils.h"
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
1) Creazione dei Thread
2) Creazione corpo della funzione Consumer
3) Creazione corpo della funzione Producer
4) Creazione della socket per la comunicazione 
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

  pthread_t threads[nThreads];
  int buff[buffSize];
  //Dichiarazione semafori 
  sem_t sem_free_slots, sem_data_items;
  xsem_init(&sem_free_slots,0,buffSize,__LINE__,__FILE__);
  xsem_init(&sem_data_items,0,0,__LINE__,__FILE__);


  pthread_mutex_t cMutex = PTHREAD_MUTEX_INITIALIZER;

  tData d;
  d.cIndex = 0;
  d.buffer = buff;
  d.cMutex = &cMutex;
  d.sem_free_slots = &sem_free_slots;
  d.sem_data_items = &sem_data_items;

  for(int i = 0; i < nThreads; i++){
    xpthread_create(&threads[i],NULL,&tbody,&d,QUI);
  }
  

	return 0;
}