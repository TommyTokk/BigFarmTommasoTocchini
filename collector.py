#! /usr/bin/python3
import socket, threading
from threading import Lock
from time import sleep
import struct as s

HOST = "127.0.0.1"
PORT = 61813

mutex = Lock()

def connectionHandler(conn, addr, dict):
    with conn:  
        print(f"Contattato da {addr}")
        # ---- Intero che rappresenta il tipo di richeista
        data = recv_all(conn,4)
        assert len(data) == 4
        req = s.unpack("!i", data[:4])[0]

        if(req == 0):
            #Richiesta di un worker per l'invio della somma e del nomeFile
            #Ricevo la somma 
            data = recv_all(conn, 8)
            assert(len(data) == 8)
            lSum = s.unpack("!q", data[:8])[0]

            #Ricevo la lunghezza del file
            data = recv_all(conn, 4)
            assert (len(data)) == 4
            lenFile = s.unpack(conn, data[:4])[0]

            #Ricevo il file
            data = recv_all(conn, lenFile)
            assert data == lenFile
            fileName = data.decode('utf-8')

            with mutex:
                if(lSum in dict):
                    if(fileName not in dict[lSum]):
                        dict[lSum].append(fileName)
                else:
                    dict[lSum] = list()
                    dict[lSum].append(fileName)
        elif req == 1:
            #Al collector arriva la richiesta di elencare tutte le coppie
            with mutex:
                #Devo inviare il numero di chiavi nel dict
                dict.sort()
                nKeys = len(dict.keys())
                conn.sendall(s.pack("!i", nKeys))

                if(nKeys != 0):
                    for keys in dict:
                        fileList = dict[keys].sort()
                        #AGGIUNGERE L'INVIO DEI DATI







        elif req == 2:
            pass

        inizio  = s.unpack("!i",data[:4])[0]
        fine = s.unpack("!i",data[4:])[0]
        print(f"Ho ricevuto i valori:", inizio,fine)
        # ---- invio risultato in formato preceduto da lunghezza
        conn.sendall(s.pack("!i",len())) #lunghezza      # singoli valori 
        print(f"Finito con {addr}")

def recv_all(conn,n):
  chunks = b''
  bytes_recd = 0
  while bytes_recd < n:
    chunk = conn.recv(min(n - bytes_recd, 1024))
    if len(chunk) == 0:
      raise RuntimeError("socket connection broken")
    chunks += chunk
    bytes_recd = bytes_recd + len(chunk)
  return chunks

def main():
    dict = {}

