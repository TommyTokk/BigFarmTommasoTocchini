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
      print("====", self.ident, "mi occupo di", self.addr)
      connectionHandler(self.conn,self.addr, self.dict)
      print("====", self.ident, "ho finito")

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

def connectionHandler(conn, addr, dict):
    with conn:  
        print(f"Contattato da {addr}\n")
        # ---- Intero che rappresenta il tipo di richeista
        data = recv_all(conn,4)
        assert len(data) == 4
        req = s.unpack("!i", data[:4])[0]
        print(f"ho ricevuto {req} come tipo di richiesta da {addr}\n")

        if(req == 0):
            #Richiesta di un worker per l'invio della somma e del nomeFile
            #Ricevo la somma 
            data = recv_all(conn, 8)
            assert(len(data) == 8)
            lSum = s.unpack("!q", data[:8])[0]

            print(f"ho ricevuto {lSum} come somma da {addr}\n")

            #Ricevo la lunghezza del file
            data = recv_all(conn, 4)
            assert (len(data)) == 4
            lenFile = s.unpack("!i", data[:4])[0]

            #Ricevo il file
            data = recv_all(conn, lenFile)
            assert len(data) == lenFile
            fileName = data.decode('utf-8')
            print(f"Ho ricevuto {fileName} come nome del file da {addr}\n")

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

                        #Invio la chiave
                        conn.sendall(s.pack("!q", keys))

                        #Invio i numero di file associati a key
                        conn.sendall(s.pack("!i"), len(fileList))

                        for file in fileList:
                            #Invio la lunghezza del nome del file
                            conn.sendall(s.pack("!", len(file)))
                            #Invio i file associati alla chiave
                            conn.sendall(file.encode())
        elif req == 2:
            #Al collector arriva la richiesta di elencare i fie con una det. somma
            #Ricevo la somma
            data = recv_all(conn, 8)
            fSum = s.unpack("!q", data[:8])[0]

            with mutex:
                try:
                    fileList = []
                    fileList = dict[fSum]
                    #Invio il numero di file presenti nella lista
                    conn.sendall(s.pack("!i", len(fileList)))

                    for file in fileList:
                        #Invio la lunghezza del nome del file
                        conn.sendall(s.pack("!i", len(file)))
                        conn.sendall(file.encode())
                except:
                    conn.sendall(s.pack("!i", 0))

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