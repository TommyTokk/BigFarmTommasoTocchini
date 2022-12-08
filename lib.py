#! /usr/bin/python3
import socket, threading
from threading import Lock
from time import sleep
import struct as s

HOST = "127.0.0.1"
PORT = 61813

mutex = Lock()

class serverThread(threading.Thread):
    def __init__(self,conn,addr, dict):
        threading.Thread.__init__(self)
        self.conn = conn
        self.addr = addr
        self.dict = dict
    def run(self):
      connectionHandler(self.conn,self.addr, self.dict)

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

def connectionHandler(conn, addr, sumDict):
    with conn:  
        # ---- Intero che rappresenta il tipo di richeista
        data = recv_all(conn,4)
        assert len(data) == 4
        req = s.unpack("!i", data[:4])[0]

        if(req == 1):
            #Richiesta di un worker per l'invio della somma e del nomeFile
            #Ricevo la somma 
            data = recv_all(conn, 8)
            assert(len(data) == 8)
            lSum = s.unpack("!q", data[:8])[0]


            #Ricevo la lunghezza del file
            data = recv_all(conn, 4)
            
            assert (len(data)) == 4
            lenFile = s.unpack("!i", data[:4])[0]

            #Ricevo il file
            data = recv_all(conn, lenFile)
            assert len(data) == lenFile
            fileName = data.decode('utf-8')
            

            with mutex:
                if(lSum in sumDict):
                    if(fileName not in sumDict[lSum]):
                        sumDict[lSum].append(fileName)
                else:
                    sumDict[lSum] = list()
                    sumDict[lSum].append(fileName)
        elif req == 2:
            #Al collector arriva la richiesta di elencare i fie con una det. somma
            #Ricevo la somma
            data = recv_all(conn, 8)
            fSum = s.unpack("!q", data[:8])[0]

            with mutex:
                try:
                    fileList = []
                    fileList = sumDict[fSum]
                    #Invio il numero di file presenti nella lista
                    conn.sendall(s.pack("!i", len(fileList)))

                    for file in fileList:
                        #Invio la lunghezza del nome del file
                        conn.sendall(s.pack("!i", len(file)))
                        #Invio il file
                        conn.sendall(file.encode())
                except:
                    conn.sendall(s.pack("!i", 0))
        elif req == 3:
            #Al collector arriva la richiesta di elencare tutte le coppie
            with mutex:
                #Devo inviare il numero di chiavi nel dict
                #sumDict = {k: v for k, v in sorted(sumDict.items(), key=lambda item: item[1])}
                sumDict = dict(sorted(sumDict.items()));
                nKeys = len(sumDict.keys())
                conn.sendall(s.pack("!i", nKeys))

                if(nKeys != 0):
                    for keys in sumDict:
                        fileList = sumDict[keys]

                        #Invio la chiave

                        conn.sendall(s.pack("!q", keys))

                        #Invio il numero di file associati a key

                        conn.sendall(s.pack("!i", len(fileList)))

                        for file in fileList:
                            #Invio la lunghezza del nome del file

                            conn.sendall(s.pack("!i", len(file)))
                            #Invio i file associati alla chiave

                            conn.sendall(file.encode())

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