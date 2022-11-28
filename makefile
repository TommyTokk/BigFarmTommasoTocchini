# Definsico il compilatore e le flag di compilazione
# Che vengono usate dalle regole implicite
CC=gcc
CFLAGS=-g -Wall -O -std=gnu99
LDLIBS=-lm -lrt -pthread

EXEC = farm
WORKER = worker
CLIENT = client
LIB = utils

# Se scriviamo solo make, verrà compilato di defult main
all: ${EXEC}

${EXEC}: ${EXEC}.o ${LIB}.o ${WORKER}.o 
$(CLIENT): $(CLIENT).o $(LIB).o


clean:
	rm -f  ${EXEC} ${CLIENT} *.o