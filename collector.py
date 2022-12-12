#!/usr/bin/python3
from lib import *

def main(host = HOST, port = PORT):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        dict = {}
        try:  
            s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            s.bind((host, port))
            s.listen()
            while True:
                # mi metto in attesa di una connessione
                conn, addr = s.accept()
                # lavoro con la connessione appena ricevuta
                # gestisci_connessione(conn,addr) 
                # t = threading.Thread(target=gestisci_connessione, args=(conn,addr))
                t = serverThread(conn,addr, dict)
                t.start()
        except KeyboardInterrupt:
            pass
            s.shutdown(socket.SHUT_RDWR)

main()