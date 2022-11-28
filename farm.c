#include "utils.h"
#include "worker.h"


// parametri del server
#define HOST "127.0.0.1"
#define PORT 61813

/*
Programma (multi-thread) che che prende dalla linea di comando
una lista di file binari e una serie di parametri.

TO DO:
1) Creazione corpo della funzione Worker
  1.1) Sistemare logica per quanto riguarda il nome del file
  1.2) Sistemare eventuali errori nel worker
  1.3) Creazione della socket per la comunicazione 
*/


volatile bool sig = false;
void handler(int s){
  if(s==SIGINT) sig = true;
}




int main(int argc, char *argv[]){

  //long tmpSum = 0;
  // controlla numero argomenti
  if(argc<2) {
      printf("Uso: %s file [file ...] \n",argv[0]);
      return 1;
  }
  struct sigaction sa;
  sa.sa_handler=&handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags=SA_RESTART;
  sigaction(SIGINT,&sa,NULL);

  //blocco tutti i segnali tranne SIGINT
  sigset_t mask;
  sigfillset(&mask);
  sigdelset(&mask,SIGINT);
  pthread_sigmask(SIG_BLOCK, &mask,NULL);
  /*
  Variabile usata per controllare se l'argomento
  in argv è un file
  */
  struct stat filePath;

  int nThreads = 4, buffSize = 8, delay = 0, opt = 0;
  int pIndex = 0, cIndex = 0;

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

  printf("nThreads = %d, buffSize = %d, delay = %d\n", nThreads, buffSize, delay);

  pthread_t *threads;
  threads = malloc(nThreads * sizeof(pthread_t));
  if(!threads) termina("Allocazione memoria thread fallito", __LINE__, __FILE__);

  char **buff = malloc(buffSize * sizeof(char *));
  if(!buff) termina ("Allocazione buffer fallita", __LINE__, __FILE__);
  //Dichiarazione semafori 
  sem_t sem_free_slots, sem_data_access;
  xsem_init(&sem_free_slots,0,buffSize,__LINE__,__FILE__);
  xsem_init(&sem_data_access,0,0,__LINE__,__FILE__);


  pthread_mutex_t cMutex = PTHREAD_MUTEX_INITIALIZER;

  Data d;

  d.cIndex = &cIndex;
  d.buffer = buff;
  d.buffSize = &buffSize;
  d.fileName = 266;
  d.cMutex = &cMutex;
  d.sem_free_slots = &sem_free_slots;
  d.sem_data_access = &sem_data_access;
 
  //Creazione dei threads
  for(int i = 0; i < nThreads; i++){
    //d[i].sommaTot = 0;
    xpthread_create(&threads[i],NULL,WorkerBody,&d,__LINE__, __FILE__);
  }

  for(int i = 1; i < argc; i++){
    if(stat(argv[i], &filePath) == -1){
      printf("non è un file\n");
      continue;//L'argomento non è un file
    }
    //Se argv[i] è un file lo carico nel buffer
    xsem_wait(&sem_free_slots, __LINE__, __FILE__);
    buff[pIndex++ % buffSize] = argv[i];
    xsem_post(&sem_data_access, __LINE__,__FILE__);
    usleep(delay*1000);
  }

  //termino i threads
  for(int i = 0; i < nThreads; i++){
    xsem_wait(&sem_free_slots, __LINE__, __FILE__);
    buff[(pIndex++) % buffSize] = NULL; //Inserisco un valore non valido per il consumatore
    xsem_post(&sem_data_access, __LINE__, __FILE__);
  }
  
  //eseguo la join dei threads
  for(int i = 0; i < nThreads; i++){
    xpthread_join(threads[i], NULL, __LINE__, __FILE__);
    //tmpSum += d[i].sommaTot;
  }


  free(buff);
  free(threads);
  sem_destroy(&sem_free_slots);
  sem_destroy(&sem_data_access);
  pthread_mutex_destroy(&cMutex);
	return 0;
}