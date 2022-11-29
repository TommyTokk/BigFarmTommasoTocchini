#include "worker.h"


#define HOST "127.0.0.1"
#define PORT 61813
/*
TODO Worker
1) Logica per la lettura del nome del file
  1.1) Aggiungere controlli sulla correttezza del nome
  1.2) Aggiungere controlli sulla correttezza della lunghezza del buffer 
  1.3) Aggiungere funzione per inviare i warning 
2) Connessione tramite socket per lo scambio di dati
  2.1) Aggiungere logica per lo scambio dati
  2.2) Aggiungere controllo degli errori
*/



void *WorkerBody(void *args){
  Data *d = (Data *) args;
  char *fName = malloc(d -> fileName);

  if(!fName) termina("Allocazione memoria nome file fallita", __LINE__, __FILE__);

  long *arrNumR = malloc(MAX_LONG * sizeof(long));

  if(!arrNumR) termina("Errore allocazione memoria arrNumR", __LINE__, __FILE__);

  long numRead, sum;

  while(true){
    xsem_wait(d -> sem_data_access,__LINE__, __FILE__);//Eseguo la wait sul semaforo per l'accesso ai dati
    xpthread_mutex_lock(d ->cMutex, __LINE__, __FILE__);//Eseguo la lock sulla mutex

    if(!(d -> buffer[*(d -> cIndex)])){
      free(fName);
      fName = NULL;
    }else{
      //ATTENZIONE ALLA DIMENSIONE DEL BUFFER
      strcpy(fName, d -> buffer[*(d -> cIndex)]);
      
      printf("Ho letto %s come nome file\n", fName);
      //ATTENZIONE ALLA DIMENSIONE DEL BUFFER

    }
    *(d -> cIndex) = (*(d -> cIndex) + 1) % *(d -> buffSize);

    xpthread_mutex_unlock(d -> cMutex, __LINE__, __FILE__);//Rilascio la mutex
    xsem_post(d -> sem_free_slots, __LINE__, __FILE__);//Eseguo la post per incrementare il semaforo degli slot liberi

    if(!fName) break;

    FILE *fin = xfopen(fName, "r", __LINE__, __FILE__);

    /*ATTENZIONE DA SISTEMARE*/
    if(fin == NULL) termina("Apertura file di input per consumer fallita", __LINE__, __FILE__);
    /*ATTENZIONE DA SISTEMARE*/

    sum = 0;
    while(!feof(fin)){
      numRead = fread(arrNumR, sizeof(long),MAX_LONG,fin); //Leggo i long dal file e gli inserisco in arrNumR
      for(int i = 0; i < numRead; i++){
        sum += i * arrNumR[i];
      }
      //d -> sommaTot += sum;
    }

    /*ATTENZIONE DA SISTEMARE*/
    if(fclose(fin) != 0) termina("Errore chiusura file input consumer", __LINE__, __FILE__);
    /*ATTENZIONE DA SISTEMARE*/

    //CREARE LA SOCKET PER LA COMUNICAZIONE
    struct sockaddr_in serverAddress;
    printf("il nome del file è %s\n", fName);
    int socket = connectionCreate(serverAddress, PORT, HOST, __LINE__, __FILE__);

    if(socket < 0){
      free(fName); 
      free(arrNumR);
      pthread_exit(NULL);
    }
    //INVIO DATI DALLA SOCKET

    //Invio il tipo di richiesta al collector
    if(sendInt(socket, 1) < 0){
      free(fName);
      free(arrNumR);
      pthread_exit(NULL);
    }

    //Invio la somma al collector
    //Siccome potrebbe non essere possibile inviare il long intero
    //Lo spezzo in due int
    if(sendLong(socket, sum) < 0){
      free(fName);
      free(arrNumR);
      pthread_exit(NULL);
    }

    int lenFileName = strlen(fName);
    assert(lenFileName > 0);
    //Invio al collector la lunghezza del nome del file
    if(sendInt(socket, lenFileName) < 0){
      free(fName);
      free(arrNumR);
      pthread_exit(NULL);
    }

    if(sendFileName(socket, fName) < 0 ){
      free(fName);
      free(arrNumR);
      pthread_exit(NULL);
    }
    if(close(socket)<0) perror("Errore chiusura socket");
  }

    free(arrNumR);
    pthread_exit(NULL);
}