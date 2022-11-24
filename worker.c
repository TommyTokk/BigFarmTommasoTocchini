#include "worker.h"
#include <arpa/inet.h>
#include <sys/socket.h>


#define HOST "127.0.0.1"
#define PORT 65432
/*
TODO Worker
1) Logica per la lettura del nome del file
  1.1) Aggiungere controlli sulla correttezza del nome
  1.2) Aggiungere controlli sulla correttezza della lunghezza del buffer 
  1.3) Aggiungere funzione per inviare i warning 
2) Connessione tramite socket per lo scambio di dati
  2.1) Aggiungere logica per lo scambio dati
  2.2) Aggiungere controllo degli errori
3) Gestire il segnale SIGINT
*/



void *WorkerBody(void *args){
  tData *d = (tData *) args;
  char *fd = malloc(d -> fileName);

  if(!fd) termina("Allocazione memoria nome file fallita");

  long *arrNumR = malloc(MAX_LONG * sizeof(long));

  if(!arrNumR) termina("Errore allocazione memoria arrNumR");

  long numRead, sum;

  do{
    xsem_wait(d -> sem_data_access,__LINE__, __FILE__);//Eseguo la wait sul semaforo per l'accesso ai dati
    xpthread_mutex_lock(d ->cMutex, __LINE__, __FILE__);//Eseguo la lock sulla mutex

    if(!(d -> buffer[d -> cIndex])){
      free(fd);
      fd = NULL;
    }else
      //ATTENZIONE ALLA DIMENSIONE DEL BUFFER
      strcpy(fd, d -> buffer[d -> cIndex % *(d -> buffSize)]);
      //ATTENZIONE ALLA DIMENSIONE DEL BUFFER
    (d -> cIndex) += 1;

    xpthread_mutex_unlock(d -> cMutex, __LINE__, __FILE__);//Rilascio la mutex
    xsem_post(d -> sem_free_slots, __LINE__, __FILE__);//Eseguo la post per incrementare il semaforo degli slot liberi

    if(!fd) break;

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
    struct sockaddr_in serverAddress;

    int socket = connectionCreate(serverAddress, PORT, HOST, __LINE__, __FILE__);

    if(socket < 0){
      free(fd);
      free(arrNumR);
      pthread_exit(NULL);
    }
    //INVIO DATI DALLA SOCKET
  }while(true);
}