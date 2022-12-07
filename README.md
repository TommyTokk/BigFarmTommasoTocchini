# Relazione progetto BigFarm - Tommaso Tocchini

All' interno del progetto gli eseguibili farm, client e worker utilizzano delle funzioni di libreria definite nel file utils.c mentre il collector, scritto in python, utilizza funzioni definite nel file lib.py.

## File utils.c
All'interno del file sono presenti delle funzioni di libreria utilizzate all'interno del progetto. Ho utilizzato il set di istruzioni fornite dal prof durante l'anno comprendenti anche il controllo degli errori; per facilitare l'invio e la ricezione dei dati sono state create delle funzioni aggiuntive:

- connectionCreate: La funzione si occupa di creare la socket utilizzata per la comunicazione. Restituisce -1 in caso di fallimento, un intero rappresentante la socket altrimenti.

- sendInt: Funzione che invia il paramtero **n** tramite la socket specificata da **fdSocket**. La funzione restituisce -1 in caso di errore 1 in caso di successo.

- sendLong: Funzione che invia il long passato come parametro tramite la socket **fdSocket**. Poiché potrebbe capitare che le dimensioni del numero da inviare siano troppo grandi ho scelto di inviare separatamente 4 byte alla volta sulla socket (lSbit ==> bit meno significativi, mSBit ==> bit più significativi). La funzione restituisce -1 in caso di fallimento, 1 in caso di successo.

- sendFileName: La funzione invia il nome del file tramite la socket. Restituisce -1 in caso di errore, 1 altrimenti.

Per ogni funzione del tipo sendType è presente la duale del tipo receive.

## Eseguibile farm
L'eseguibile farm è così composto;
Il masterWorker ha il compito di leggere una lista di file dalla linea di comando e di valutare l'eventuale presenza di parametri opzionali (-q, -n, -t) tramite la funzione **getopt**. Il masterWorker inizializza la struttura dati per la gestione del segnale SIGINT. Successivamente si occuperà della creazione dei thread worker (è possibile scegliere il numero di thread tramite il parametro opzionale -n, il valore di default è 4). Lo scambio di informazioni tra masterWorker e threads worker avviene tramite lo schema produttore/consumatore. In particolare:
il masterWorker legge dei nomi di file dalla linea di comando, tramite la funzione **stat** controlla che l'elemento letto sia un file, in caso affermativo scrive il nome all'interno del buffer condiviso. Per regolare l'accesso al buffer sono presenti due semafori:

- **sem_free_slots**: Indica il numero di posti liberi all'interno del buffer
- **sem_data_access**: Indica il numero di posti occupati

inseriti i valori all'interno del buffer, il masterWorker si occuperà anche di inserire dei valori di terminazione per i threads.

Una volta scritti i valori per la terminazione dei threads all'inerno del buffer condiviso il masterWorker si metterà in attesa dei threads tramite una join.

## Thread worker
Come detto in precedenza i threads worker svolgeranno il ruolo di consumatori. Il loro compito sarà quello di prelevare il nome del file dal buffer condiviso, aprirlo in lettura e calcolarne la somma corrispondente. Una volta effettuato ciò ogni worker invierà al client, tramite la socket creata dalla funzione **connectioCreate** i vari dati. Ogni thread termina non appena legge dal buffer il valore illegale NULL. Nel caso in cui dovessero esserci errori viene terminato solo il thread in questione così che gli altri possano continuare a lavorare.

### Thread worker: Invio dei dati
Per quanto riguarda l'invio dei dati per prima cosa viene inviato il tipo di richiesta (1), successivamente, tramite le funzioni citate sopra, avviene l'invio dei dati al collector.

## Collector
Il collector è in grado di gestire tre tipi di richieste, la prima dai threads worker le restanti dal client:

- Tipo 1: Al collector viene richiesto di memorizzare le coppie (somma, nomeFile). il collector attende di ricevere una coppia dal worker. I valori vengono memorizzati all'inerno di un dizionario avente come chiavi le somme e come valori una lista di file con la stessa somma.
    - Un esempio potrebbe essere {-1 : ["z1.dat", "s-1"]}

- Tipo 2: Una delle due richieste provenienti dal client. In questo caso viene chiesto al server di restituire tutti i file con una determinata somma.

- Tipo 3: Al server viene chiesto di elencare tutti i file, con le relative somme. Il server restituisce al client quest'ultimi in ordine crescente rispetto alla somma.

Tenendo conto che il server sfutta il multithreading l'accesso al dizionario viene eseguito in mutua esclusione.

Il server rimane sempre in ascolto di eventuali nuove connessioni

## Client
Il processo client effettua due tipi di richieste al server, Nel primo caso richiede al server tutti i file con una determinata somma mentre nel secondo richiede tutte le somme, con i relativi file, memorizzati all'interno del server. In entrambi i casi il client stampa ciò che ha ricevuto sullo standard output nel formato "somma nomeFile"
