#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <unistd.h>  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <sys/stat.h>  
#include <sys/wait.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <pthread.h>  
#include <time.h>  
#define N 5 //Numero di righe  
#define M 5 //Numero di colonne  
#define Z N*M+N*2+M //Numero di elementi della mia matrice  
int visits=0;  
int sockd[220];  
char *mesi[] = {"Gennaio", "Febbraio", "Marzo", "Aprile", "Maggio", "Giugno", "Luglio", "Agosto", "Settembre", "Ottobre", "Novembre", "Dicembre"};  
int vittoria=0;  
char userVittoria[24];  
char vittoriaMessage[1024];   
struct mappa **matrice;    
struct utenti *top=NULL; 
struct utenti *nuovo(struct utenti *top,char *user, char *pw);
struct utenti *nuovomappa(struct utenti *mappa,char *user, char *pw,int color);
struct mappa **nuovaMappa(struct mappa **a,char *user,char *pw,int i, int j);
struct mappa **inizializzaMatrice(struct mappa **a);
int getij(struct utenti *top, char *user, int opzione);
int getcolor(struct utenti *top,char *user);
void setij(struct utenti *top,char *user,int i ,int j);
int int_rand(int min, int max);
int inserisciBandiere(struct mappa **a, int riga, int colonna);
void riempiMatrice(struct mappa **a);
int inizializzaGiocatore(struct utenti *top,char *user, int i); 
struct utenti *inserisciColore(struct utenti *top, int color, char *user);
int inserisciSoldato(struct mappa **a,char *user,char *passw,int i , int j, int color,int k);
int ricerca(struct utenti *top,char *user);
void eliminatoFile(char *user);
int controlloPercorso( struct mappa **a,int riga, int colonna);
int login(struct utenti *top,char *user,char*pw);
void deallocaMatrice(struct mappa **a);
void vita(struct utenti *top, char *user,int i);
void eliminaVita(struct utenti *mappa,int color);
struct utenti *elimina(struct utenti *top,char *user);
int spostamento(struct mappa **a, char *user, char *passw, int c);
int start(struct utenti *top);
void disconnect(struct utenti *top,char *user);
void disconnectall(struct utenti *top);
void richiesta(int *socket_desc) ;
void scriviFile(int i);
void stampaMatrice(struct mappa **a);
void stampa(struct utenti *top);
char *utentiConnessi(struct utenti *top,char *users);
char *alleati(struct utenti *top,char *user,char *stringa);
void stampamatrix(struct utenti *top,char *matrix,char *username,int i1,int j1,int k);
char *stampaostacoli(struct mappa **a,char *matrix);





void win(char *mex,int sock){  
    char numero[20];  
    int i;  
    for(i=0;i<=visits;i++){  
        
        if(i%2!=0){   
            if(sockd[i]!=sock){ 
                write(sockd[i],mex,120);  
                
            }  
        }  
        
    }  
}    
//creo le strutture che mi servono matrice ,utenti 
struct mappa{  
    /*Valore conterrà informazioni riguardanti la partita, in particolare 
     *    0 - Posizione Libera. 
     *    1 - Posizione Occupata da uno ostacolo. 
     *    2 - Posizione Occupata da una bandiera. 
     *    3 - Posizione Occupata da un giocatore. 
     */  
    int valore;  
    
    char *descrizione;  // Variabile per maggiori informazioni riguardante il valore (es:colore bandiera)  
    struct utenti *utente;  
    //Eventuale contatore.  
    int x;  
};  


//Struttura per memorizzare informazioni su gli utenti registrati alla piattaforma  
//l'utente ha una password la vita che serve per restare in gioco il colore della squadra di appartenenza 
struct utenti {  
    char username[30];  
    char password[30];  
    int vita;  
    int colore; //1 - Blu ; 2 - Rosso ; 0 - Utente registrato ma ancora deve entrare in gioco ; -1 Utente Eliminato.  
    int i;  
    int j;  
    int connected;  
    struct utenti *next;  
};  
//-------------------------------------------------------------------------------------------------------------------------------------
//creo gli algoritmi di settaggio delle informazioni 

//Inserimento in coda di una lista  aggiunge alla coda della lista un nuovo utente semplicemente attacca alla fine della lista l'utente tmp 
struct utenti *incoda(struct utenti *top, struct utenti *tmp){  
    if(top==NULL){  
        tmp->next=NULL;  
        top=tmp;  
        return tmp;  
    }  
    else {  
        tmp=incoda(top->next,tmp);  
        top->next=tmp;  
        return top;  
    }  
}  

int getij(struct utenti *top, char *user, int opzione){  
    // opzione : 0 - Ritorno i , 1 - Ritorno j  
    int x;  
    if(opzione==0){  
        while(top!=NULL){  
            if(strcmp(top->username,user)==0){  
                x=top->i;  
            }  
            top=top->next;  
        }  
    }  
    if(opzione==1){  
        while(top!=NULL){  
            if(strcmp(top->username,user)==0){  
                x=top->j;  
            }  
            top=top->next;  
        }  
    }  
    return x;  
}  
//prende il colore di appartenenza a una squadra compara la stringa username e user che indicano l'utente e preleva il colore   
int getcolor(struct utenti *top,char *user){  
    int x;  
    while(top!=NULL){  
        if(strcmp(top->username,user)==0){  
            x=top->colore;  
        }  
        top=top->next;  
    }  
    return x;  
}  


void setij(struct utenti *top,char *user,int i ,int j){  
    while(top!=NULL){  
        if(strcmp(top->username,user)==0){  
            top->i=i;  
            top->j=j;  
        }  
        top=top->next;  
    }  
}  

//funzione per prendere casualmente un numero in un intervallo   
int int_rand(int min, int max){  
    int numero_casuale;  
    int differenza;  
    differenza=(max-min)+1;  
    numero_casuale=rand() % differenza;  
    numero_casuale=numero_casuale+min;  
    return numero_casuale;  
}

//funzione per inserire la bandierina
int inserisciBandiere(struct mappa **a, int riga, int colonna){  
    int i,j;  
    int result=0;  
    if(a[riga][colonna].valore==1 || a[riga][colonna].valore==2) result=1;  
    return result;  
} 


void riempiMatrice(struct mappa **a){  
    int muroMin,muroMax,muro,contatore=0;  
    int i,j;  
    //alcuni elementi sono caratterizzati da un muro  
    muroMin=N*M*10/100;  
    muroMax=N*M*20/100;  
    srand((unsigned int) time(0));  
    //Numero di caselle dedicate al muro da inserire nella mia matrice.  
    muro=int_rand(muroMin,muroMax);  
    //Posiziono ostacoli all'interno della mia matrice in maniera random  
    while(contatore < muro){  
        i= rand() % N; //Numero Random tra le righe  
        j= rand() % M; //Numero Random tra le colonne  
        if(controlloPercorso(a,i,j)==0){  
            a[i][j].valore=1;  
            contatore++;  
        }  
    }  
    //Setto il contatore a 0  
    contatore=0;  
    //Procedo ad inserire le due bandiere in maniera random.  
    while(contatore <2){  
        i= rand() % N;  
        j= rand() % M;  
        if(inserisciBandiere(a,i,j)==0){  
            a[i][j].valore=2;  
            a[i][j].descrizione=(char*)malloc(sizeof(char));  
            if(contatore==0){  
                strcpy(a[i][j].descrizione,"Bandiera Blu");  
            }  
            if(contatore==1){  
                strcpy(a[i][j].descrizione,"Bandiera Rossa");  
            }  
            contatore ++;  
        }  
    }  
    
}  


//Funzione per inserire un nuovo elemento all'interno della lista  
struct utenti *nuovo(struct utenti *top,char *user, char *pw){  
    struct utenti *tmp=(struct utenti *) malloc(sizeof(struct utenti));  
    strcpy(tmp->username,user);  
    strcpy(tmp->password,pw);  
    tmp->colore=0;  
    tmp->connected=0;  
    tmp->next=NULL;  
    top=incoda(top,tmp);  
    return top;  
}  
//funzione per inserire una nuova mappa e relativi elementi ????
struct utenti *nuovomappa(struct utenti *mappa,char *user, char *pw,int color){  
    struct utenti *tmp=(struct utenti *) malloc(sizeof(struct utenti));  
    strcpy(tmp->username,user);  
    strcpy(tmp->password,pw);  
    tmp->colore=color;  
    tmp->next=NULL;  
    mappa=incoda(mappa,tmp);  
    return mappa;  
}  


struct mappa **nuovaMappa(struct mappa **a,char *user,char *pw,int i, int j){  
    int k;  
    struct mappa **tmp=(struct mappa **) malloc (sizeof(struct mappa*));  
    for(k=0;k<N;k++){  
        tmp[k]=(struct mappa*) malloc(sizeof(struct mappa));  
    }  
    strcpy(tmp[i][j].utente->username,user);  
    strcpy(tmp[i][j].utente->password,pw);  
    tmp[i][j].utente->next=NULL;  
    top=incoda(a[i][j].utente,tmp[i][j].utente);  
    return a;  
    
}  
struct mappa **inizializzaMatrice(struct mappa **a){  
    //Definisco Indice di riga e indice di colonna  
    int i,j;  
    //Alloco la mia matrice  
    a=(struct mappa**)malloc(sizeof(struct mappa*)*N);  
    for(i=0;i<N;i++){  
        a[i]=(struct mappa*)malloc(sizeof(struct mappa)*M);  
        for(j=0;j<M;j++){  
            //Setto tutti gli elementi della mia matrice a 0 (Posizione Libera)  
            a[i][j].valore=0;  
            a[i][j].utente=NULL;  
        }  
    }  
    return a;  
}  


int inizializzaGiocatore(struct utenti *top,char *user, int i){  
    int flag=0;  
    while(top!=NULL){  
        //Scorro la mia lista fino a trovare l'utente che ha effettuato il login alla piattaforma  
        if(strcmp(top->username,user)==0){  
            if(i==0){  
                //Verifico se non ha ancora effettuato l'inserimento sulla mappa  
                if ( top->colore == 0 ){  
                    flag=1;  
                }  
            }  
            if(i==1 || i==2){  
                top->colore=i;  
                top->vita=3000;  
                flag=1;  
            }  
            
        }  
        //Scorro la mia lista per trovare il mio username  
        top=top->next;  
    }  
    return flag;  
} 

struct utenti *inserisciColore(struct utenti *top, int color, char *user){  
    while(top!=NULL){  
        if(strcmp(top->username,user)==0){  
            top->colore=color;  
        }  
        top=top->next;  
    }  
    return top;  
}  

int inserisciSoldato(struct mappa **a,char *user,char *passw,int i , int j, int color,int k){  
    // 1 - Inserimento Non riuscito in quanto vi è un ostacolo  
    // 2 - Inserimento Effettuato con successo.  
    // 3 - Inserimento Effettuato con successo ed eliminato i punti vita a gli utenti di un altro colore.  
    int flag=0;  
    char colore[6];  
    //Controllo che i valori passati siano compresi nel range delle mie variabili.  
    if(i<N && j < M){  
        //Controllo se la partita non è terminata  
        if(a[i][j].valore==2){  
            color=getcolor(top,user);  
            if(color==1 && strcmp(a[i][j].descrizione,"Bandiera Rossa")==0 || color==2 && strcmp(a[i][j].descrizione,"Bandiera Blu")==0){  
                
                vittoria=1;  
                flag=8;  
                if(color==1){ strcpy(colore,"Blu"); scriviFile(1);}  
                else{   strcpy(colore,"Rosso"); scriviFile(2);}  
                strcpy(userVittoria,user);  
                strcpy(vittoriaMessage,"La partita e stata vinta da: ");  
                strcat(vittoriaMessage,user);  
                strcat(vittoriaMessage,"\nLa squadra ");  
                strcat(vittoriaMessage,colore);  
                strcat(vittoriaMessage," ha vinto la partita.");  
                strcat(vittoriaMessage,"\0");  
                
            }  
            else{  
                a[i][j].utente=nuovomappa(a[i][j].utente,user,passw,color);  
                setij(top,user,i,j);  
                flag=2;  
            }  
            
            
        }  
        //Controllo se vi è un muro         
        
        else if(a[i][j].valore==1){  
            if(k==0){  
                a[i][j].utente=nuovomappa(a[i][j].utente,user,passw,color);  
                setij(top,user,i,j);  
                flag=2;  
            }  
            else{            
                a[i][j].x=1;  
                flag=1;  
            }  
        }  
        
        //Altrimenti Procedo con l'inserimento.  
        else if(a[i][j].utente==NULL){  
            //Inserisci una funzione per inserire all'interno anche il colore del mio soldato  
            a[i][j].utente=nuovomappa(a[i][j].utente,user,passw,color);  
            setij(top,user,i,j);  
            flag=2;  
        }  
        //Nel caso in cui vi è anche un altro soldato sulla mappa, controllo se sono alleati o nemici  
        //Se sono alleati l'effetto sarà nullo, altrimenti il nemico perderà 500 punti vita.  
        else if( a[i][j].utente!=NULL){  
            //Scorro La lista degli utenti presenti in questa posizione  
            while(a[i][j].utente!=NULL){  
                //Verifico se ci sono utenti diversi da quello principale  
                //Verifico il colore  
                if(a[i][j].utente->colore!=color){  
                    //Procedo ad eliminare la vita  
                    eliminaVita(a[i][j].utente,color);  
                    flag=3;  
                }  
                
                a[i][j].utente=a[i][j].utente->next;  
            }  
            if(flag!=3) flag=2;  
            
            
            a[i][j].utente=nuovomappa(a[i][j].utente,user,passw,color);  
            setij(top,user,i,j);  
        }  
    }else flag=0; 
    
    
    return flag;  
}  


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------  
//algoritmi per la ricerca cancellazione e altro   
//Ricerca di un utente all'interno della lista semplice scanzione di una lista con confronto che riceve in ingresso la lista e un user ritorna uno se cè corrispondenza
int ricerca(struct utenti *top,char *user){  
    int i=0;  
    struct utenti *tmp;  
    tmp=top;  
    while(tmp!=NULL){  
        if(strcmp(tmp->username,user)==0){  
            i=1;  
        }  
        tmp=tmp->next;  
    }  
    return i;  
    
}  
// struttura per eliminare il file di log dopo la chiusura del gioco   
void eliminatoFile(char *user){  
    FILE *log;  
    time_t rawtime;  
    time (&rawtime);  
    struct tm* leggibile;  
    leggibile = localtime(&rawtime);  
    log=fopen("log.txt","a");  
    fprintf(log,"%d/%s/%d %d:%d:%d L'utente %s è stata eliminato\n",leggibile->tm_mday,mesi[leggibile->tm_mon],leggibile->tm_year + 1900,leggibile->tm_hour,leggibile->tm_min,leggibile->tm_sec,user);  
    fclose(log);  
}  


int controlloPercorso( struct mappa **a,int riga, int colonna){  
    int i,j; //scorrono la matrice servono per i for  
    int c1=0,c2=0;  
    int result=0;  
    for(j=0;j < M ; j++){  
        if(a[riga][j].valore==1) c2++;  
    }  
    for (i=0; i < N ; i++){  
        if(a[i][colonna].valore==1) c1++;  
    }  
    if(c1 >= N-1 || c2 >= M-1) result=1;  
    //Verifico che non inserisca una cella vuota all'interno della mia matrice seguito da muri  
    for(i=0;i<N;i++){  
        for(j=0;j<M;j++){  
            if(i+2 < N && i-1 > -1 && j-1 > -1 && j+1 < M) {  
                if(a[i+2][j].valore == 1 && a[i-1][j-1].valore==1 && a[i+1][j+1].valore==1){  
                    result=0;  
                }  
            }  
            if(i-2 > -1 && i+1 < N && j-1 > -1 && j+1 < M){  
                if(a[i-2][j].valore==1 && a[i-1][j-1].valore==1 && a[i+1][j+1].valore==1){  
                    result=0;  
                }  
            }  
            if(j+2 < M && j-1 > -1 && i-1 > -1 && i+1 < N){  
                if(a[i][j+2].valore==1 && a[i-1][j-1].valore==1 && a[i+1][j+1].valore==1){  
                    result=0;  
                }  
            }  
            if(j-2 > -1 && j+1 < N && i-1 > -1 && i+1 < M){  
                if(a[i][j-2].valore==1 && a[i-1][j-1].valore==1 && a[i+1][j+1].valore==1){  
                    result=0;  
                }  
            }  
            if(i==0){  
                if(a[i+1][j].valore==1 && a[i][j+1].valore==1){  
                    result=0;  
                }  
            }  
            else if(i==N-1){  
                if(a[i-1][j].valore==1 && a[i][j+1].valore==1){  
                    result=0;  
                }  
            }  
            else if(j==0){  
                if(a[i][j-1].valore==1 && a[i+1][j].valore==1){  
                    result=0;  
                }  
            }  
            else if(j==M-1){  
                if(a[i-1][j].valore==0 && a[i][j-1].valore==1){  
                    result=0;  
                }  
            }  
            
            
        }  
    }  
    
    return result;  
}  





//Funzione per il login, controlla se corrispondono username e password all'interno della lista  
int login(struct utenti *top,char *user,char*pw){  
    int i=0;  
    while(top!=NULL){  
        if(strcmp(top->username , user) == 0 && strcmp(top->password , pw) == 0){  
            top->connected=1;  
            i=1;  
        }  
        top=top->next;  
    }  
    return i;  
}  




void deallocaMatrice(struct mappa **a){  
    int i,j;  
    for(i=0;i<N;i++){  
        free(a[i]);  
    }  
    free(a);  
}  



void vita(struct utenti *top, char *user,int i){  
    while(top!=NULL){  
        if(strcmp(top->username,user)==0){  
            if(i==0){  
                top->vita=top->vita-500;  
                if(top->vita <1) eliminatoFile(user);  
            }  
            if(i==1){  
                top->vita=top->vita -1;  
            }  
        }  
        top=top->next;  
    }  
}  

//Funzione Per Eliminare Vita agli utenti del gioco.  
void eliminaVita(struct utenti *mappa,int color){  
    char *user;  
    //Scorro la lista degli utenti presenti sulla posizione sulla mappa  
    while(mappa!=NULL){  
        //Se il colore degli utenti presenti in quella posizione è diverso da quello dell'utente che arriva in quella posizione procedo  
        //a decrementare la vita  
        if(mappa->colore!=color){  
            user=(char * )malloc(sizeof(char));  
            strcpy(user,mappa->username);  
            //Alloco spazio per la variabile che mi conterrà le informazioni dell utente presente sulla mappa  
            //Scorro la mia lista degli utenti per trovare l'utente trovato e decrementargli la vita  
            vita(top,user,0);                
        }    
        
        mappa=mappa->next;  
    }  
    
}  



struct utenti *elimina(struct utenti *top,char *user){  
    struct utenti *tmp;  
    if(top!=NULL){  
        if(strcmp(top->username,user)==0){  
            tmp=top->next;  
            free (top);  
            top=tmp;  
        }  
        else  top->next=elimina(top->next,user);  
    }  
    return top;  
}  

int verificaVita(struct utenti *top, char *user){  
    int flag=0;  
    while(top!=NULL){  
        if(strcmp(top->username,user)==0){  
            if(top->vita > 0) flag=1;  
        }  
        top=top->next;  
    }  
    return flag;  
}  




int spostamento(struct mappa **a, char *user, char *passw, int c){  
    char prova[2];  
    int flag,color,k,i,j;  
    i=getij(top,user,0);  
    j=getij(top,user,1);  
    color=getcolor(top,user);  
    if(vittoria==1){  
        flag=9;  
        c=9;  
    }  
    else if(verificaVita(top,user)==0){  
        c=10;  
        flag=10;  
    }  
    switch(c){  
        case 1:  
            if(i-1 <= -1){  
                flag=4;  
                break;  
            }  
            flag=inserisciSoldato(a,user,passw,i-1,j,color,1);  
            vita(top,user,1);  
            a[i][j].utente=elimina(a[i][j].utente,user);  
            break;  
        case 2:  
            if(i+1 > N-1 ){  
                flag=5;  
                break;  
            }  
            flag=inserisciSoldato(a,user,passw,i+1,j,color,1);  
            vita(top,user,1);  
            a[i][j].utente=elimina(a[i][j].utente,user);  
            break;  
        case 3:  
            if(j+1 > M-1){  
                flag=6;  
                break;  
            }  
            flag=inserisciSoldato(a,user,passw,i,j+1,color,1);  
            vita(top,user,1);            
            a[i][j].utente=elimina(a[i][j].utente,user);  
            break;  
        case 4:  
            if(j-1 <= -1){  
                flag=7;  
                break;  
            }  
            flag=inserisciSoldato(a,user,passw,i,j-1,color,1);  
            vita(top,user,1);  
            a[i][j].utente=elimina(a[i][j].utente,user);  
            break;  
        case 9: //Vitorria da parte di un utente.  
            break;  
        case 10: //Utente non ha abbastanza vita per spostarsi.  
            break ;  
        default:  
            //Qualcosa è andato storto.  
            break;  
    }  
    return flag;  
    
    
}  




//funzione per vedere se cè almeno un giocatore per squadra  
int start(struct utenti *top){  
    
    int k=0;
    int j=0;  
    
    while(top!=NULL){  
        if(top->connected==1){  
            
            if(top->colore==1){  
                k++;
            }  
            else if(top->colore==2){  
                j++;
            }  
            
        }  
        top=top->next;  
    }  
    if (k>0&&j>0){
        return 1;
    }
    return 0;  
}  



void disconnect(struct utenti *top,char *user){  
    while(top!=NULL){  
        if(strcmp(top->username,user)==0){  
            top->connected=0;  
        }  
        top=top->next;  
    }  
}  

void disconnectall(struct utenti *top){  
    while(top!=NULL){  
        top->colore=0;  
        top->i=0;  
        top->j=0;  
        top->vita=0;  
        top->connected=0;  
        top=top->next;  
    }  
}  



void richiesta(int *socket_desc)  
{  
    //Informazioni per gestire la richiesta.  
    int sock = *(int*)socket_desc;  
    
    int i,j,k,i1,j1,eliminazione;  
    int read_size,flag=1,opzione,muro;  
    char a,*message,*message2 ,matrix[Z],numero[1], client_message[1024],client2_message[1024], *username, *password,*user,*pw,prova[2];  
    FILE *fp;    
    message2=(char *)malloc(sizeof(char));  
    message=(char*)malloc(sizeof(char));  
    username=(char*)malloc(sizeof(char));  
    
    while(flag){  
        
        if(vittoria==1){  
            win(vittoriaMessage,sock);  
            deallocaMatrice(matrice);  
            disconnectall(top);  
            matrice=inizializzaMatrice(matrice);  
            riempiMatrice(matrice);  
            stampaMatrice(matrice);  
            
            vittoria=0;  
        }  
        
        if(read_size = recv(sock , client_message ,sizeof(client_message), 0)){  
            opzione=atoi(client_message);  
            switch (opzione){  
                case 1:  
                    write(sock,"Inserisci Username\0",sizeof("Inserisci Username\0"));  
                    strncpy(client_message," ",sizeof(client_message));  
                    if(read_size=recv(sock,client_message,sizeof(client_message), 0)){  
                        username=(char *)malloc(sizeof(client_message));  
                        strcpy(username,client_message);  
                        write(sock,"Inserisci Password\0",sizeof("Inserisci Password\0"));  
                        strncpy(client_message," ",sizeof(client_message));  
                        
                        if(read_size=recv(sock,client_message,sizeof(client_message),0)){  
                            password=(char*)malloc(sizeof(client_message));  
                            strcpy(password,client_message);  
                        }  
                    }  
                    
                    
                    if(login(top,username,password)==1){  
                        write(sock,"Login Effettuato\0",sizeof("Login Effettuato\0"));  
                        
                        
                    }  
                    else{  
                        
                        write(sock,"Login Fallito\0",sizeof("Login Fallito\0"));  
                    }  
                    break;  
                    
                case 2:  
                    write(sock,"Inserisci Username:\0",sizeof("Inserisci Username:\0"));  
                    strncpy(client_message," ",sizeof(client_message));  
                    if(read_size=recv(sock,client_message,sizeof(client_message), 0)){  
                        username=(char *)malloc(sizeof(client_message));  
                        strcpy(username,client_message);  
                        if(ricerca(top,username)) {  
                            write(sock,"Username gia presente\0", sizeof("Username gia presente\0"));  
                            break;
                            
                        }
                    }
                    write(sock,"Inserisci Password\0",sizeof("Inserisci Password\0"));  
                    strncpy(client_message," ",sizeof(client_message));  
                    
                    if(read_size=recv(sock,client_message,sizeof(client_message),0)){  
                        password=(char*)malloc(sizeof(client_message));  
                        strcpy(password,client_message);  
                    }  
                    fp=fopen("user.txt","a");  
                    if(fp==NULL){  
                        puts("Problema con apertura file");  
                        break;  
                    }  
                    fprintf(fp,"%s:%s\n",username,password);  
                    fclose(fp);  
                    
                    write(sock,"Registrazione effettuata con succeso\0",sizeof("Registrazione effettuata con successo\0"));  
                    
                    //Inserisco I miei utenti all'interno di una linked list.  
                    top=nuovo(top,username,password);  
                    
                    break;  
                    
                    
                case 3:  
                    write(sock," ", sizeof(" "));  
                    strcpy(client_message," ");  
                    strcpy(client2_message," ");  
                    if(read_size=recv(sock,client2_message,sizeof(client2_message), 0)){  
                        username=(char*)malloc(sizeof(client_message));  
                        strcpy(username,client2_message);  
                        if(inizializzaGiocatore(top,username,0)==1){  
                            write(sock,"1",sizeof("1"));  
                            strcpy(client_message," ");  
                            if(read_size=recv(sock,client_message,sizeof(client_message),0)){  
                                //Devo effettuare l'inserimento del colore delle armate.  
                                opzione=atoi(client_message);  
                                
                                if(inizializzaGiocatore(top,username,opzione)==1){  
                                    //Ho impostato la squadra per l'username  
                                    while (start(top)!=1){
                                        
                                        sleep(2);
                                        
                                    }
                                    
                                    stampamatrix(top,matrix,username,i1,j1,1);  
                                    write(sock,matrix,strlen(matrix));  
                                    
                                    if(read_size=recv(sock,client_message,sizeof(client_message),0)){  
                                        
                                        
                                        
                                        
                                        write(sock,"Inserisci Riga dove posizionare il giocatore\0",sizeof("Inserisci Riga dove posizionare il giocatore\0"));  
                                        strcpy(client_message," \0");                            
                                        if(read_size=recv(sock,client2_message,2,0)){  
                                            //Ricevuto la posizione dove inserire il giocatore  
                                            i=atoi(client2_message);  
                                            
                                            
                                        }  
                                        write(sock,"Inserisci Colonna dove posizionare il giocatore\0",sizeof("Inserici Colonna dove posizionare il giocatore\0"));  
                                        
                                        if(read_size=recv(sock,client2_message,2,0)){  
                                            j=atoi(client2_message);  
                                        }  
                                        if(inserisciSoldato(matrice,username,password,i,j,opzione,0)>1)  
                                        {  
                                            //Ha inserito Correttamente il soldato.  
                                        }  
                                        
                                        
                                        
                                        
                                        
                                        
                                    }  
                                }  
                            }  
                        }  
                        else write(sock,"0",sizeof("0"));  
                    }  
                    
                    break;  
                    case 4://Spostamento  
                        
                        
                        
                        write(sock,"Dove ti vuoi spostare?\n1 - Nord\n2 - Sud\n3 - Est\n4 - Ovest\0" , sizeof("Dove ti vuoi spostare?\n1 - Nord\n2 - Sud\n3 - Est\n4 - Ovest\0"));  
                        strcpy(client_message," ");  
                        //Ricevo informazioni dal client riguardo lo spostamento.  
                        if(read_size=recv(sock,client_message,sizeof(client_message),0)){  
                            opzione=atoi(client_message);  
                            write(sock," ", sizeof (" "));  
                            strncpy(client_message," ",sizeof(client_message));  
                            if(read_size=recv(sock,client_message,sizeof(client_message),0)){  
                                username=(char *)malloc(sizeof(client_message));  
                                strcpy(username,client_message);  
                                write(sock," ",sizeof(" "));  
                                strcpy(client_message," ");  
                                if(read_size=recv(sock,client_message,sizeof(client_message),0)){  
                                    password=(char *) malloc(sizeof(client_message));  
                                    strcpy(password,client_message);  
                                    strcpy(client_message," ");  
                                    //Richiamo una funzione che mi effettua lo spostamento e mi decrementa di uno la vita del mio personaggio.  
                                    //Poichè siamo in presenza di un server concorrente, lo spostamento è stato effettuato tenendo conto di alcune considerazioni:  
                                    //In ogni posizione , la mappa avrà solamente l'ultimo giocatore che è arrivato in quel punto, in modo tale, da evitare di poter eliminare il giocatore avversario con mosse di tipo nord - sud , est-ovest e viceversa.  
                                    //Nel caso in cui quindi si verifichi una collisione, il giocatore avversario perderà 500 punti vita e verrà temporaneamente eliminato dalla mappa, mantenendo le informazioni riguardanti la sua precedente posizione.  
                                    flag=spostamento(matrice,username,password,opzione);  
                                    switch(flag){  
                                        case 1:  
                                            write(sock,"Non e' stato potuto completare lo spostamento in quanto e' presente un ostacolo\0",sizeof("Non e' stato potuto completare lo spostamento in quanto e' presente un ostacolo\0"));  
                                            break;  
                                        case 2:  
                                            write(sock,"Spostamento effettuato con successo\0",sizeof("Spostamento effettuato con successo\0"));  
                                            break;  
                                        case 3:  
                                            write(sock,"Lo Spostamento ha portato ad una collisione con altri giocatori appartenenti alla squadra nemica che hanno perso 500 punti vita\0",sizeof("Lo Spostamento ha portato ad una collisione con altri giocatori appartenenti alla squadra nemica che hanno perso 500 punti vita\0"));  
                                            break;  
                                        case 4:  
                                            write(sock,"Spostamento fallito!\nNon e' possibile andare ancora a Nord \0",sizeof("Spostamento fallito!\nNon è possibile andare ancora a Nord \0"));  
                                            break;  
                                        case 5:  
                                            write(sock,"Spostamento fallito!\nNon e' possibile andare ancora a Sud \0",sizeof("Spostamento fallito!\nNon è possibile andare ancora a Sud \0"));  
                                            break;  
                                        case 6:  
                                            write(sock,"Spostamento fallito!\nNon e' possibile andare ancora a Est \0",sizeof("Spostamento fallito!\nNon e' possibile andare ancora a Est \0"));  
                                            break;  
                                        case 7:  
                                            write(sock,"Spostamento fallito!\nNon e' possibile andare ancora a Ovest \0",sizeof("Spostamento fallito!\nNon è possibile andare ancora a Ovest \0"));  
                                            break;  
                                        case 8:  
                                            write(sock,"1\0",sizeof("1\0"));  
                                            break;  
                                        case 9:  
                                            write(sock,vittoriaMessage,strlen(vittoriaMessage));  
                                            break;  
                                        case 10:  
                                            write(sock,"Sei stato eliminato,non puoi spostarti\0",sizeof("Sei stato eliminato,non puoi spostarti\0"));  
                                        default:  
                                            break;  
                                    }  
                                }  
                                
                            }  
                        }  
                        
                        
                        
                        break;  
                                        case 5:
                                            write(sock,"Disconnessione in corso...\0",sizeof("Disconessione in corso...\0"));  
                                            if(read_size=recv(sock,client_message,sizeof(client_message),0)){  
                                                strcpy(username,client_message);   
                                                disconnect(top,username);  
                                            }  
                                            break;  
                                        case 6:  
                                            message2=NULL; 
                                            message2=utentiConnessi(top,message2);               
                                            //Stampa degli utenti collegati.  
                                            write(sock,message2,strlen(message2));  
                                            
                                            break;  
                                        case 7://Stampa degli ostacoli  
                                            write(sock," ",sizeof(" "));  
                                            if(read_size=recv(sock,client_message,sizeof(client_message),0)){  
                                                strcpy(username,client_message);  
                                                message=stampaostacoli(matrice,message);  
                                                write(sock,message,strlen(message));  
                                            }  
                                            
                                            break;  
                                        case 8:  
                                            write(sock," ",sizeof(" "));  
                                            if(read_size=recv(sock,client_message,sizeof(client_message),0)){  
                                                strcpy(username,client_message);  
                                                message2=alleati(top,username,message2);  
                                                write(sock,message2,strlen(message2));  
                                            }  
                                            
                                            break;  
                                        case 9:  
                                            //Mi ricavo le informazioni su dove si trova l'utente e gli stampo *  
                                            i1=getij(top,username,0);  
                                            j1=getij(top,username,1);  
                                            write(sock," ", sizeof (" "));  
                                            strncpy(client_message," ",sizeof(client_message));  
                                            if(read_size=recv(sock,client_message,sizeof(client_message),0)){  
                                                strcpy(username,client_message);  
                                                
                                                stampamatrix(top,matrix,username,i1,j1,0);  
                                                write(sock,matrix,strlen(matrix));  
                                            }  
                                            break;  
                                        default:  
                                            write(sock,"Comando non riconosciuto\0",sizeof("Comando non riconosciuto\0"));  
                                            break;  
                                            
                                            
            }  
        }  
        
        
        
        if(read_size == 0)  
        {  
            
        }  
        else if(read_size == -1)  
        {  
            perror("recezione fallita");  
        }  
    }  
    
    //Libero il puntatore.  
    free(socket_desc);  
    
    
}  
//funzione per scrivere i file di log    
void scriviFile(int i){  
    char colore[6];  
    FILE *log;  
    time_t rawtime;  
    time (&rawtime);  
    struct tm* leggibile;  
    leggibile = localtime(&rawtime);  
    if(i==1) strcpy(colore,"Blu");  
    else if(i==2) strcpy(colore,"Rosso");  
    log=fopen("log.txt","a");  
    fprintf(log,"%d/%s/%d %d:%d:%d La Bandiera %s è stata trovata.\n",leggibile->tm_mday,mesi[leggibile->tm_mon],leggibile->tm_year + 1900,leggibile->tm_hour,leggibile->tm_min,leggibile->tm_sec,colore);  
    fprintf(log,"%d/%s/%d %d:%d:%d Partita Terminata\n",leggibile->tm_mday,mesi[leggibile->tm_mon],leggibile->tm_year + 1900,leggibile->tm_hour,leggibile->tm_min,leggibile->tm_sec);  
    fprintf(log,"%d/%s/%d %d:%d:%d Nuova Partita Generata\n",leggibile->tm_mday,mesi[leggibile->tm_mon],leggibile->tm_year + 1900,leggibile->tm_hour,leggibile->tm_min,leggibile->tm_sec);  
    fclose(log);  
    
}    
//------------------------------------------------------------------------------------------------------------------------------------
//funzioni di stampa 

void stampaMatrice(struct mappa **a){  
    FILE *fp;  
    fp=fopen("mappa.txt","w");  
    int i,j;  
    for(i=0;i<N;i++){  
        for(j=0;j<M;j++){  
            fprintf(fp,"%d ",a[i][j].valore);  
        }  
        fprintf(fp,"\n");  
    }  
    fclose(fp);  
}  

//Stampa della lista lato server, per debug  
void stampa(struct utenti *top){  
    int i=0;  
    while(top!=NULL){  
        puts(top->username);  
        puts(top->password);  
        top=top->next;  
    }  
}
char *utentiConnessi(struct utenti *top,char *users){  
    users=NULL;
    users=(char * )malloc(2000*sizeof(char));
    int k;  
    char numero[5];  
    strcpy(users,"Utenti Connessi Alla Piattaforma:\n");  
    while(top!=NULL){  
        if(top->connected==1){  
            strcat(users,"Utente: ");  
            strcat(users,top->username);  
            strcat(users," Squadra di appartenenza: ");  
            if(top->colore==1){  
                strcat(users,"BLU");  
            }  
            else if(top->colore==2){  
                strcat(users,"ROSSO");  
            }  
            strcat(users," Punti Vita: ");  
            k=snprintf(numero,5,"%d",top->vita);  
            strcat(users,numero);  
            strcat(users,"\n");  
        }  
        top=top->next;  
    }  
    
    return users;  
}  

char *alleati(struct utenti *top,char *user,char *stringa){  
    stringa=realloc(stringa,2000*sizeof(char));  
    int k;  
    char numero[5];  
    int color;  
    color=getcolor(top,user);  
    strcpy(stringa,"Soldati Alleati:\n");  
    
    while(top!=NULL){  
        if(strcmp(top->username,user)!=0){  
            if(top->colore==color){  
                strcat(stringa,"Utente: ");  
                strcat(stringa,top->username);  
                strcat(stringa," Posizione : (");  
                snprintf(numero,3,"%d",top->i);  
                strcat(stringa,numero);  
                strcat(stringa,",");  
                snprintf(numero,3,"%d",top->j);  
                strcat(stringa,numero);  
                strcat(stringa,") ");  
                strcat(stringa," Punti Vita: ");  
                snprintf(numero,5,"%d",top->vita);  
                strcat(stringa,numero);  
                strcat(stringa,"\n");  
            }  
        }  
        top=top->next;  
    }  
    return stringa;  
}  

void stampamatrix(struct utenti *top,char *matrix,char *username,int i1,int j1,int k){  
    int i,j;  
    char numero[5];   
    if(k==1){  
        i1=-2;  
        j1=-2;  
    }  
    strcpy(matrix,"Mappa Della Partita: \n");  
    for(i=-1;i<N;i++){  
        for(j=-1;j<M;j++){  
            if(i==-1 && j==-1){  
                strcat(matrix,"x ");  
            }  
            else if(i==-1){  
                snprintf(numero,3,"%d",j);  
                strcat(matrix,numero);  
                strcat(matrix," ");  
            }  
            
            else if(j==-1) {  
                snprintf(numero,3,"%d",i);  
                strcat(matrix,numero);  
                strcat(matrix," ");  
            }  
            else if(i==i1 && j==j1) strcat(matrix,"o ");  
            
            else strcat(matrix,"- ");      
            
        }  
        strcat(matrix,"\n");  
    }  
    
    strcat(matrix,"\0");  
    
    
}  



char *stampaostacoli(struct mappa **a,char *matrix){  
    
    matrix=realloc(matrix,2000*sizeof(char));  
    strcpy(matrix,"Stampo la mappa degli ostacoli incontrati:\n-x: Rappresenta posizione in cui e'stato incontrato un ostacolo\n");  
    int i,j,k;  
    char numero[5];  
    for(i=-1;i<N;i++){  
        for(j=-1;j<M;j++){  
            if(i==-1 && j==-1){  
                strcat(matrix,"x ");  
            }  
            else if(i==-1){  
                k=snprintf(numero,3,"%d",j);  
                strcat(matrix,numero);  
                strcat(matrix," ");  
            }  
            
            else if(j==-1) {  
                k=snprintf(numero,3,"%d",i);  
                strcat(matrix,numero);  
                strcat(matrix," ");  
            }  
            else if(a[i][j].x==1) strcat(matrix,"x ");  
            else strcat(matrix,"- ");      
            
        }  
        strcat(matrix,"\n");  
    }  
    strcat(matrix,"\0");  
    return matrix;  
}  


//-------------------------------------------------------------------------------------------------------------------------
//main     

int main (int argc, char *argv[]){  
    FILE *log;  
    int socket_desc,client_sock,c,read_size,t;  
    struct sockaddr_in server,client[20];  
    char str[20];  
    int t_free[16];  
    int n;  
    pthread_t tid;  
    pthread_t t_id[16];  
    pthread_t daeid[16];  
    pthread_attr_t attr;  
    struct utenti *top=NULL;  
    time_t rawtime;  
    time (&rawtime);  
    struct tm* leggibile;  
    FILE *fp;  
    //All'apertura del server creo il mio file contenente gli utenti che si registreranno alla mia applicazione.  
    fp=fopen("user.txt","w");  
    if(fp==NULL){  
        perror("errore file");  
        return 1;  
    }  
    
    matrice=inizializzaMatrice(matrice);  
    
    riempiMatrice(matrice);  
    
    stampaMatrice(matrice);  
    
    //Controllo input  
    if(argc < 2){  
        write(STDERR_FILENO,"Attenzione, sintassi errata.\n Eseguire il programma selezionando la porta\n",sizeof("Attenzione, sintassi errata.\n Eseguire il programma selezionando la porta\n"));  
        exit(1);  
    }  
    //Creo il mio socket  
    socket_desc=socket(AF_INET,SOCK_STREAM,0);  
    if(socket_desc == -1){  
        write(STDERR_FILENO,"Impossibile creare socket\n",sizeof("Impossibile creare socket\n"));  
        return 1;  
    }  
    puts("Socket Creata");  
    
    //Creo la struttura del mio server  
    server.sin_family=AF_INET;  
    server.sin_addr.s_addr=INADDR_ANY;  
    server.sin_port=htons(atoi(argv[1]));  
    
    //Effettuo la Bind  
    if(bind(socket_desc,(struct sockaddr * )&server,sizeof(server))<0){  
        perror("Bind fallita");  
        return 1;  
    }  
    puts("In attesa di connessioni...");  
    //Effettuo listen  
    listen(socket_desc,16);  
    leggibile = localtime(&rawtime);  
    log=fopen("log.txt","a");     
    fprintf(log,"%d/%s/%d %d:%d:%d  Nuova Partita Generata.\n",leggibile->tm_mday,mesi[leggibile->tm_mon],leggibile->tm_year + 1900,leggibile->tm_hour,leggibile->tm_min,leggibile->tm_sec);  
    fclose(log);  
    while(1){  
        leggibile = localtime(&rawtime);  
        n=sizeof(client);  
        
        if((client_sock=accept(socket_desc,(struct sockaddr *)&client[visits],&n))< 0){  
            perror("accept failed\n");  
            exit(1);  
        }  
        
        sockd[visits]=client_sock;  
        visits++;  
        
        log=fopen("log.txt","a");  
        
        
        
        inet_ntop(AF_INET,&client[visits].sin_addr,str,INET_ADDRSTRLEN);  
        if(visits%2==0){  
            fprintf(log,"%d/%s/%d %d:%d:%d  Nuova connessione da client %2s \n",leggibile->tm_mday,mesi[leggibile->tm_mon],leggibile->tm_year + 1900,leggibile->tm_hour,leggibile->tm_min,leggibile->tm_sec,inet_ntoa(client[visits-1].sin_addr));  
        }
        fclose(log);  
        
        
        
        if(pthread_create(&tid,NULL,(void *)richiesta,&client_sock)){            
            perror("Impossibile avviare un thread");  
            return 0;  
        }  
        pthread_detach(tid);  
        
        if(client_sock<0){  
            perror("Accettazione fallita");  
            return 1;  
        }  
    }   
    
    return 0;  
}  

