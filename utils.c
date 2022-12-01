#include "utils.h"
// collezione di chiamate a funzioni di sistema con controllo output




// termina un processo con eventuale messaggio d'errore
void termina(const char *messaggio, int linea, char *file) {
	if(errno==0)  fprintf(stderr,"== %d == %s Linea:%d, File: %s\n",getpid(), messaggio, linea, file);
	else fprintf(stderr,"== %d == %s: %s\n",getpid(), messaggio,
              strerror(errno));
  exit(1);
}

void terminaThread(const char *messaggio, int linea, char *file){
  if(errno==0)  fprintf(stderr,"== %d == %s Linea:%d, File: %s\n",getpid(), messaggio, linea, file);
  else fprintf(stderr,"== %d == %s: %s\n",getpid(), messaggio,
              strerror(errno));
  pthread_exit(NULL);
}


// ---------- operazioni su FILE *
FILE *xfopen(const char *path, const char *mode, int linea, char *file) {
  FILE *f = fopen(path,mode);
  if(f==NULL) {
    perror("Errore apertura file");
    fprintf(stderr,"== %d == Linea: %d, File: %s\n",getpid(),linea,file);
    exit(1);
  }
  return f;
}

// ----------- operazioni su file descriptors
void xclose(int fd, int linea, char *file) {
  int e = close(fd);
  if(e!=0) {
    perror("Errore chiusura file descriptor");
    fprintf(stderr,"== %d == Linea: %d, File: %s\n",getpid(),linea,file);
    exit(1);
  }
  return;
}


// -------------- operazioni su processi
pid_t xfork(int linea, char *file)
{
  pid_t p = fork();
  if(p<0) {
    perror("Errore fork");
    fprintf(stderr,"== %d == Linea: %d, File: %s\n",getpid(),linea,file);
    exit(1);
  }
  return p;
}

pid_t xwait(int *status, int linea, char *file)
{
  pid_t p = wait(status);
  if(p<0) {
    perror("Errore wait");
    fprintf(stderr,"== %d == Linea: %d, File: %s\n",getpid(),linea,file);
    exit(1);
  }
  return p;
}


int xpipe(int pipefd[2], int linea, char *file) {
  int e = pipe(pipefd);
  if(e!=0) {
    perror("Errore creazione pipe"); 
    fprintf(stderr,"== %d == Linea: %d, File: %s\n",getpid(),linea,file);
    exit(1);
  }
  return e;
}

// ---------------- memoria condivisa POSIX
int xshm_open(const char *name, int oflag, mode_t mode, int linea, char *file)
{
  int e = shm_open(name, oflag, mode);
  if(e== -1) {
    perror("Errore shm_open"); 
    fprintf(stderr,"== %d == Linea: %d, File: %s\n",getpid(),linea,file);
    exit(1);
  }
  return e;  
}

int xshm_unlink(const char *name, int linea, char *file)
{
  int e = shm_unlink(name);
  if(e== -1) {
    perror("Errore shm_unlink"); 
    fprintf(stderr,"== %d == Linea: %d, File: %s\n",getpid(),linea,file);
    exit(1);
  }
  return e;  
}

int xftruncate(int fd, off_t length, int linea, char *file)
{
  int e = ftruncate(fd,length);
  if(e== -1) {
    perror("Errore ftruncate"); 
    fprintf(stderr,"== %d == Linea: %d, File: %s\n",getpid(),linea,file);
    exit(1);
  }
  return e;  
}

void *simple_mmap(size_t length, int fd, int linea, char *file)
{
  void *a =  mmap(NULL, length ,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
  if(a == (void *) -1) {
    perror("Errore mmap"); 
    fprintf(stderr,"== %d == Linea: %d, File: %s\n",getpid(),linea,file);
    exit(1);
  }
  return a;
}

int xmunmap(void *addr, size_t length, int linea, char *file)
{
  int e = munmap(addr, length);
  if(e== -1) {  
    perror("Errore munmap"); 
    fprintf(stderr,"== %d == Linea: %d, File: %s\n",getpid(),linea,file);
    exit(1);
  }
  return e;
}


// ---- semafori POSIX
sem_t *xsem_open(const char *name, int oflag, mode_t mode, 
              unsigned int value,  int linea, char *file) {
  sem_t *s = sem_open(name,oflag,mode,value);
  if (s==SEM_FAILED) {
    perror("Errore sem_open");
    fprintf(stderr,"== %d == Linea: %d, File: %s\n",getpid(),linea,file); 
    exit(1);
  }
  return s;
}

// chiude un NAMED semaphore 
int xsem_close(sem_t *s, int linea, char *file)
{
  int e = sem_close(s);
  if(e!=0) {
    perror("Errore sem_close"); 
    fprintf(stderr,"== %d == Linea: %d, File: %s\n",getpid(),linea,file);
    exit(1);
  }
  return e;  
}

int xsem_unlink(const char *name, int linea, char *file)
{
  int e = sem_unlink(name);
  if(e!=0) {
    perror("Errore sem_unlink"); 
    fprintf(stderr,"== %d == Linea: %d, File: %s\n",getpid(),linea,file);
    exit(1);
  }
  return e;  
}


int xsem_init(sem_t *sem, int pshared, unsigned int value, int linea, char *file) {
  int e = sem_init(sem,pshared,value);
  if (e!=0) {
    perror("Errore sem_init");
    fprintf(stderr,"== %d == Linea: %d, File: %s\n",getpid(),linea,file);
    pthread_exit(NULL);
  }
  return e;
}

int xsem_post(sem_t *sem, int linea, char *file) {
  int e = sem_post(sem);
  if (e!=0) {
    perror("Errore sem_post");
    fprintf(stderr,"== %d == Linea: %d, File: %s\n",getpid(),linea,file);
    pthread_exit(NULL);
  }
  return e;
}

int xsem_wait(sem_t *sem, int linea, char *file) {
  int e = sem_wait(sem);
  if (e!=0) {
    perror("Errore sem_wait");
    fprintf(stderr,"== %d == Linea: %d, File: %s\n",getpid(),linea,file);
    pthread_exit(NULL);
  }
  return e;
}




// ----- thread (non scrivono il codice d'errore in errno)
#define Buflen 100
void xperror(int en, char *msg){
  char buf[Buflen];
  
  char *errmsg = strerror_r(en, buf, Buflen);
  if(msg!=NULL)
    fprintf(stderr,"%s: %s\n",msg, errmsg);
  else
    fprintf(stderr,"%s\n",errmsg);
}


int xpthread_create(pthread_t *thread, const pthread_attr_t *attr,
                          void *(*start_routine) (void *), void *arg, int linea, char *file) {
  int e = pthread_create(thread, attr, start_routine, arg);
  if (e!=0) {
    xperror(e, "Errore pthread_create");
    fprintf(stderr,"== %d == Linea: %d, File: %s\n",getpid(),linea,file);
    pthread_exit(NULL);
  }
  return e;                       
}

                          
int xpthread_join(pthread_t thread, void **retval, int linea, char *file) {
  int e = pthread_join(thread, retval);
  if (e!=0) {
    xperror(e, "Errore pthread_join");
    fprintf(stderr,"== %d == Linea: %d, File: %s\n",getpid(),linea,file);
    pthread_exit(NULL);
  }
  return e;
}


// mutex 
int xpthread_mutex_init(pthread_mutex_t *restrict mutex, const pthread_mutexattr_t *restrict attr, int linea, char *file) {
  int e = pthread_mutex_init(mutex, attr);
  if (e!=0) {
    xperror(e, "Errore pthread_mutex_init");
    fprintf(stderr,"== %d == Linea: %d, File: %s\n",getpid(),linea,file);
    pthread_exit(NULL);
  }  
  return e;
}

int xpthread_mutex_destroy(pthread_mutex_t *mutex, int linea, char *file) {
  int e = pthread_mutex_destroy(mutex);
  if (e!=0) {
    xperror(e, "Errore pthread_mutex_destroy");
    fprintf(stderr,"== %d == Linea: %d, File: %s\n",getpid(),linea,file);
    pthread_exit(NULL);
  }
  return e;
}

int xpthread_mutex_lock(pthread_mutex_t *mutex, int linea, char *file) {
  int e = pthread_mutex_lock(mutex);
  if (e!=0) {
    xperror(e, "Errore pthread_mutex_lock");
    fprintf(stderr,"== %d == Linea: %d, File: %s\n",getpid(),linea,file);
    pthread_exit(NULL);
  }
  return e;
}

int xpthread_mutex_unlock(pthread_mutex_t *mutex, int linea, char *file) {
  int e = pthread_mutex_unlock(mutex);
  if (e!=0) {
    xperror(e, "Errore pthread_mutex_unlock");
    fprintf(stderr,"== %d == Linea: %d, File: %s\n",getpid(),linea,file);
    pthread_exit(NULL);
  }
  return e;
}

ssize_t readn(int fd, void *ptr, size_t n) {  
   size_t   nleft;
   ssize_t  nread;
 
   nleft = n;
   while (nleft > 0) {
     if((nread = read(fd, ptr, nleft)) < 0) {
        if (nleft == n) return -1; /* error, return -1 */
        else break; /* error, return amount read so far */
     } else if (nread == 0) break; /* EOF */
     nleft -= nread;
     ptr   += nread;
   }
   return(n - nleft); /* return >= 0 */
}


/* Write "n" bytes to a descriptor */
ssize_t writen(int fd, void *ptr, size_t n) {  
   size_t   nleft;
   ssize_t  nwritten;
 
   nleft = n;
   while (nleft > 0) {
     if((nwritten = write(fd, ptr, nleft)) < 0) {
        if (nleft == n) return -1; /* error, return -1 */
        else break; /* error, return amount written so far */
     } else if (nwritten == 0) break; 
     nleft -= nwritten;
     ptr   += nwritten;
   }
   return(n - nleft); /* return >= 0 */
}


//Funzione che crea la connessione alla socket
int connectionCreate(struct sockaddr_in serverAddress, int PORT, char *HOST, int line, char *fileErr){
  int fdSk = 0;
  if ((fdSk = socket(AF_INET, SOCK_STREAM, 0)) < 0){
    perror("Creazione socket fallita");
    fprintf(stderr,"== %d == Linea: %d, File: %s\n",getpid(),line,fileErr);
    return -1;
  }

  serverAddress.sin_family = AF_INET;
  serverAddress.sin_port = htons(PORT);//Converto il tipo della porta in network order
  serverAddress.sin_addr.s_addr = inet_addr(HOST);

  if (connect(fdSk, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0){ //apro la connessione
    perror("Apertura connessione fallita");
    fprintf(stderr,"== %d == Linea: %d, File: %s\n",getpid(),line,fileErr);
    return -1;
  }

  return fdSk;
}

int sendInt(int fdSocket, int n){
  int e;
  int num = htonl(n);

  e = writen(fdSocket, &num, sizeof(int));
  if(e == -1) termina("Errore writen", __LINE__, __FILE__);
  if(e != sizeof(int)){
    fprintf(stderr, "Errore writen in linea:%d, file:%s", __LINE__, __FILE__);
    return -1;
  }

  return 1;
}

int sendLong(int fdSocket, long num){
  //Invio il long in due int separati
  int e;
  int lSBit = num, mSBit = num >> 32;

  mSBit = htonl(mSBit);
  e = writen(fdSocket, &mSBit, sizeof(int));
  if(e == -1) termina("Errore writen", __LINE__, __FILE__);
  if(e != sizeof(int)){
    fprintf(stderr, "Errore writen, Linea:%d, File: %s\n", __LINE__, __FILE__);
    return -1;
  }

  lSBit = htonl(lSBit);
  e = writen(fdSocket, &lSBit, sizeof(int));
  if(e == -1) termina("Errore writen", __LINE__, __FILE__);
  if(e != sizeof(int)){
    fprintf(stderr, "Errore writen, Linea:%d, File: %s\n", __LINE__, __FILE__);
    return -1;
  }

  return 1;
}

int sendFileName(int fdSocket, char *fileName){
  int e, len;

  if(!fileName){
    fprintf(stderr, "Invio stringa fallito. nome file inesistente, Linea:%d, File: %s\n", __LINE__, __FILE__);
    return -1;
  }

  len = strlen(fileName);
  e = writen(fdSocket, fileName, len);

  if(e == -1) termina("Errore invio nome del file", __LINE__, __FILE__);
  if(e != len){
    fprintf(stderr, "Invio nome file fallito, Linea:%d, File: %s\n", __LINE__, __FILE__);
    return -1;
  }

  return 1;
}

int reciveInt(int fdSocket, int *num){
  int e;
  e = readn(fdSocket, num, sizeof(int));

  if(e == -1) termina("Errore readn", __LINE__, __FILE__);
  if(e != sizeof(int)){
    fprintf(stderr, "Errore readn, Linea:%d, File: %s\n", __LINE__, __FILE__);
    return -1;
  }

  *num = ntohl(*num);
  return 1;
}

int reciveLong(int fdSocket, long *num){
  int e, lSBit, mSbit;

  e = readn(fdSocket, &mSbit, sizeof(int));
  if(e == -1) termina("Errore readn", __LINE__, __FILE__);
  if(e != sizeof(int)){
    fprintf(stderr, "Errore readn, Linea:%d, File: %s\n", __LINE__, __FILE__);
    return -1;
  }
  mSbit = ntohl(mSbit);

  e = readn(fdSocket, &lSBit, sizeof(int));
  if(e == -1) termina("Errore readn", __LINE__, __FILE__);
  if(e != sizeof(int)){
    fprintf(stderr, "Errore readn, Linea:%d, File: %s\n", __LINE__, __FILE__);
    return -1;
  }

  lSBit = ntohl(lSBit);
  *num = ((long)mSbit << 32) + (lSBit & LONG_MASK);

  return 1;
}

int reciveFileName(int fdSocket, char *str, int fileLength){
  int e;

  /*if(!str){
    fprintf(stderr, "Lettura stringa non riuscita, Linea:%d, File: %s\n", __LINE__, __FILE__);
    return -1;
  }*/
  e = readn(fdSocket, str, fileLength);
  if(e == -1) termina("Lettura nome file fallita", __LINE__, __FILE__);
  if(e != fileLength){
    fprintf(stderr, "Errore readn, Linea:%d, File: %s\n", __LINE__, __FILE__);
    return -1;
  }
  return 1;
}

//FUNZIONI CLIENT
long xstrtol(const char *nptr, char **endptr, int base, int linea, char *file){
    long res = strtol(nptr, endptr,base);

    if(res == 0){
      if (errno == EINVAL)//Errore durante la conversione
        termina("Errore durante la conversione str --> long, LINEA : %d, FILE: %s\n", linea, file);
    }

    if(res == LONG_MIN || res == LONG_MAX){
      if(errno == ERANGE) fprintf(stderr, "Valore out of bound, LINEA: %d, FILE: %s", linea, file);
    }

    return res;
}
void freeArray(char **file, int dim){
  for(int i = 0; i < dim; i++){
    free(file[i]);
  }

  free(file);
}

void stampaArrayFile(char **arrayFile, int nFiles, long key){
  for(int i = 0; i < nFiles; i++){
    fprintf(stdout, "%ld | %s\n", key, arrayFile[i]);
  }

}
