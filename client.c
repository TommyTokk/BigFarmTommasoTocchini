#include "utils.h"

/*
TODO:
1) Inviare le richieste al server
*/
int main(int argc, char *argv[]){
    
    //long params;
    int dim = argc - 1;
    long *sumArray;
    char *endptr;
    
    sumArray = malloc(dim * sizeof(long));
    if(!sumArray) termina("Creazione array fallita", __LINE__, __FILE__);
    
    for(int i = 0; i < dim; i++){
        sumArray[i] = xstrtol(argv[i+1], &endptr, 10, __LINE__, __FILE__);
    }

    free(sumArray);
    return 0;
}
