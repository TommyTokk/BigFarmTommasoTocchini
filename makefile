# Definsico il compilatore e le flag di compilazione
# Che vengono usate dalle regole implicite
CC=gcc
CFLAGS=-g -Wall -O -std=gnu99
LDLIBS=-lm -lrt -pthread

EXEC = farm
WORKER = worker
CLIENT = client
SERVER = collector
LIB = utils

# Se scriviamo solo make, verrà compilato di defult main
all: ${EXEC} ${CLIENT}

${EXEC}: ${EXEC}.o ${LIB}.o ${WORKER}.o 
$(CLIENT): $(CLIENT).o $(LIB).o

server:
	./${SERVER}.py &

kill:
	pkill -9 ${SERVER}.py

clean:
	rm -f  ${EXEC} ${CLIENT}  *.o