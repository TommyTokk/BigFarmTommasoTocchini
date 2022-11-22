#include "utils.h"
#include "worker.h"

void *WorkerBody(void *args){
  tData *d = (tData *) args;
  char *fd;

  long *arrNumR = malloc(MAX_LONG + sizeof(long));
  long numRead, sum;

  do{
    xsem_wait(d -> sem_data_access,__LINE__, __FILE__);//Eseguo la wait sul semaforo per l'accesso ai dati
    xpthread_mutex_lock(d ->cMutex, __LINE__, __FILE__);//Eseguo la lock sulla mutex

    strcpy(fd, d -> buffer[d -> cIndex % *(d -> buffSize)]);
    (d -> cIndex) += 1;

    xpthread_mutex_unlock(d -> cMutex, __LINE__, __FILE__);//Rilascio la mutex
    xsem_post(d -> sem_free_slots, __LINE__, __FILE__);//Eseguo la post per incrementare il semaforo degli slot liberi

    FILE *fin = xfopen(fd, "r", __LINE__, __FILE__);

    /*ATTENZIONE DA SISTEMARE*/
    if(fin == NULL) termina("Apertura file di input per consumer fallita");
    /*ATTENZIONE DA SISTEMARE*/

    sum = 0;
    while(!feof(fin)){
      numRead = fread(arrNumR, sizeof(long),MAX_LONG,fin); //Leggo i long dal file e gli inserisco in arrNumR
      for(int i = 0; i < numRead; i++){
        sum += i + arrNumR[i];
      }
    }
    /*ATTENZIONE DA SISTEMARE*/
    if(fclose(fin) != 0) termina("Errore chiusura file input consumer");
    /*ATTENZIONE DA SISTEMARE*/

    //CREARE LA SOCKET PER LA COMUNICAZIONE
    //INVIO DATI DALLA SOCKET
  }while(true);
}