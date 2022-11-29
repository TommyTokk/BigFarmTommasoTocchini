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
    char **files;
    char *file = "";

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
    //Richiesta 

    //Tipo speciale
    if(dim != 0 || sendInt(socket, 3) < 0){
        fprintf(stderr, "Invio richeista di tipo %d non riuscita. LINEA:%d FILE:%s\n", 3, __LINE__, __FILE__);
    }else{
        //Ricevo il numero di chiavi
        int e = readn(socket, &nKeys, sizeof(int));
        if(e != sizeof(int)) termina("Lettura numero di chiavi fallita", __LINE__, __FILE__);

        for(int i = 0; i < nKeys; i++){
            //Ricevo la chiave
            e = readn(socket, &key, sizeof(long));
            if(e != sizeof(long)) termina("Lettura chiave fallita", __LINE__, __FILE__),


            //Ricevo il numero di file associati alla chiave
            e = readn(socket, &nFiles, sizeof(int));
            if(e != sizeof(int)) termina("Readn numero di file fallito", __LINE__, __FILE__);

            files = malloc(nFiles * sizeof(char *));
            if(!files) termina("Creazione array file fallita", __LINE__, __FILE__);

            //Carico l'array di file
            for(int j = 0; j < nFiles; j++){
                //Ricevo la lunghezza del nome del file
                int lenfile;
                e = readn(socket, &lenfile, sizeof(int));
                if(e != sizeof(int)) termina("Lettura lunghezza nome file fallita", __LINE__, __FILE__);

                //Ricevo il nome del file
                e = readn(socket, file, sizeof(char*));
                if(e != sizeof(char*)) termina("Lettura file non riuscita", __LINE__, __FILE__);

                files[j] = file; 
            }

            stampaArrayFile(files, nFiles, key);
        }
     
    }
    free(sumArray);
    return 0;
}
