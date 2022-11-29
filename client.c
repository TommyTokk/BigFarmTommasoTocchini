#include "utils.h"
#define HOST "127.0.0.1"
#define PORT 61813

/*
TODO:
1) Inviare le richieste al server
    2) Correggere la comunicazione client -> collector
*/
int main(int argc, char *argv[]){
    
    //long params;
    int dim = argc - 1;
    //int reqType = dim == 0 ? 3 : 2;
    long *sumArray;
    char *endptr;

    int nKeys = 0, nFiles = 0;
    long key = 0;    
    char **files = NULL;
    char *file = NULL;

    /*files = malloc(1 * sizeof(char*));
    if(!files) termina("Allocazione memoria array file fallita \n", __LINE__,__FILE__);*/

    sumArray = malloc(dim * sizeof(long));
    if(!sumArray) termina("Creazione array fallita", __LINE__, __FILE__);
    
    for(int i = 0; i < dim; i++){
        sumArray[i] = xstrtol(argv[i+1], &endptr, 10, __LINE__, __FILE__);
    }

    struct sockaddr_in serverAddress;
    int socket = connectionCreate(serverAddress, PORT, HOST, __LINE__, __FILE__);

    if(socket < 0){
        free(sumArray);
        termina("Errore creazione socket", __LINE__, __FILE__);
    }

    //Invio il tipo di richiesta

    switch (dim){
    case 0:
        if(sendInt(socket, 3) < 0) termina("Invio richiesta di tipo 3 non riuscita.\n", __LINE__, __FILE__);
        //Ricevo il numero di chiavi
        if(reciveInt(socket, &nKeys) < 0) termina("Lettura numero di chiavi fallita", __LINE__, __FILE__);
        //fprintf(stdout,"Ricevuto numero di chiavi pari a :%d\n", nKeys);

        for(int i = 0; i < nKeys; i++){
            //Ricevo la chiave
            if(reciveLong(socket, &key) < 0) termina("Lettura chiave fallita", __LINE__, __FILE__);
            //fprintf(stdout,"Ricevuta chiave :%ld\n", key);

            //Ricevo il numero di file associati alla chiave
            if(reciveInt(socket, &nFiles) < 0) termina("Readn numero di file fallito", __LINE__, __FILE__);
            //fprintf(stdout,"File associati alla chiave %ld :%d\n", key, nFiles);
            assert(nFiles > 0);
            files = malloc(nFiles * sizeof(char *));
            if(!files) termina("Creazione array file fallita", __LINE__, __FILE__);

            //Carico l'array di file
            for(int j = 0; j < nFiles; j++){
                //Ricevo la lunghezza del nome del file
                int lenfile;

                if(reciveInt(socket, &lenfile) < 0) termina("Readn numero di file fallito", __LINE__, __FILE__);
                //fprintf(stdout,"Ricevuta lunghezza del file :%d\n", lenfile);
                //Ricevo il nome del file
                file = malloc(lenfile * sizeof(char));
                if(!file) termina("Allocazione memoria file fallita\n", __LINE__, __FILE__);

                if(reciveFileName(socket, file, lenfile) < 0)termina("Lettura file non riuscita", __LINE__, __FILE__); 
                //fprintf(stdout,"Ricevuto file :%s\n", file);
                file[lenfile] = '\0';
                files[j] = file; 
            }
            stampaArrayFile(files, nFiles, key);
            freeArray(files, nFiles);           
        }
        
        break;
    case 1://Richiesta di tipo 2
        if(sendInt(socket, 2) < 0) termina("Invio richiesta di tipo 2 non riuscita.\n", __LINE__, __FILE__);
        long sum = xstrtol(argv[1], &endptr, 10, __LINE__, __FILE__);
        if(sendLong(socket, sum) < 0) termina("Invio somma da cercare fallita\n", __LINE__, __FILE__);

        //Ricevo il numero di file associato alla chiave
        if(reciveInt(socket, &nFiles) < 0) termina("Readn numero di file fallito", __LINE__, __FILE__);
        if(nFiles == 0){
            fprintf(stdout, "Nessun File\n");
            break;
        }
        assert(nFiles > 0);
        files = malloc(nFiles * sizeof(char *));
        if(!files) termina("Creazione array file fallita", __LINE__, __FILE__);

        //Carico l'array di file
        for(int j = 0; j < nFiles; j++){
            //Ricevo la lunghezza del nome del file
            int lenfile;

            if(reciveInt(socket, &lenfile) < 0) termina("Readn numero di file fallito", __LINE__, __FILE__);
            //fprintf(stdout,"Ricevuta lunghezza del file :%d\n", lenfile);
            //Ricevo il nome del file
            file = malloc(lenfile * sizeof(char));
            if(!file) termina("Allocazione memoria file fallita\n", __LINE__, __FILE__);

            if(reciveFileName(socket, file, lenfile) < 0)termina("Lettura file non riuscita", __LINE__, __FILE__); 
            //fprintf(stdout,"Ricevuto file :%s\n", file);
            file[lenfile] = '\0';
            files[j] = file; 
        }

        stampaArrayFile(files, nFiles, sum);
        freeArray(files, nFiles);
        break;
    }
    
    free(sumArray);
    return 0;
}
