#include<stdio.h> //printf
#include <stdlib.h>
#include<string.h>    //strlen
#include<sys/socket.h>    //socket
#include<arpa/inet.h> //inet_addr
#include <termios.h>
#include <unistd.h>
#include <pthread.h>
int flag;

void premenu(){
    puts("Per poter giocare devi registrarti e successivamente loggare ");
    puts("il gioco consiste ad un simil ruba bandiera i player si muoveranno allo scuro ");
    puts("Che Operazione vuoi eseguire?\n1-Accedere\n2-Registrarti\n");
}
void loggedmenu(){
    puts(" Benvenuto nel gioco\n Regole\n a- ad una collisione con un ostacolol’effetto sarà nullo;");
    puts(" b-nella locazione di un altro utente nemico, l’utente nemico perderà  500punti ");
    puts(" c-nella locazione della bandiera nemica l’effetto sarà la vittoria\n");
    puts("Che Operazione vuoi eseguire?\n1-Spostamento\n2-Disconettiti\n3-Visualizza Utenti Collegati\n4-Visualizza Ostacoli Incontrati\n5-Posizioni soldati della propria squadra con vita rimanente\n6-Stampa Mappa");
}




void connettiti(int sock){
    int scelta,indice=0,n,i;
    char message[1000] , server_reply[2000], server_reply2[2000],server_reply3[2000],server_reply4[2000],server_reply5[2000],server_reply6[2000];
    char comando[2],username[24],password[24],posizione[5];
    char *pw,ch;
    ssize_t nchr=0;
    pthread_t figlio;
    
    do{
        
        do{
            system("clear");
            premenu();
            while(1){
                scanf("%s",comando);
                scelta=atoi(comando);
                if(scelta==1 || scelta==2) break;
                puts("Inserisci un operazione valida.\n");
            }
            switch (scelta){
                
                
                case 1:
                    strcpy(server_reply," ");
                    if( send(sock , comando , strlen(comando) , 0) < 0){
                        puts("Send failed");
                        break;
                    }
                    
                    strcpy(server_reply," ");
                    if( recv(sock , server_reply , 2000 , 0) < 0){
                        puts("Errore ricezione messaggio");
                        break;
                    }
                    puts(server_reply);
                    scanf("%s",username);
                    if(send(sock,username,strlen(username),0) < 0){
                        puts("Errore comunicazione server");
                        break;
                    }
                    if(recv(sock,server_reply,2000,0) < 0){
                        puts("Errore ricezione messaggio");
                        break;
                    }
                    puts(server_reply);
                    pw=getpass("");
                    
                    strcpy(password,pw);
                    if(send(sock,pw,strlen(pw), 0) < 0 ){
                        puts("Errore comunicazione server");
                        break;
                    }
                    strcpy(server_reply," ");
                    if(recv(sock,server_reply,2000,0) < 0){
                        puts("Errore ricezione messaggio");
                        break;
                    }  
                    
                    if(strcmp(server_reply,"Login Fallito")==0){
                        
                        flag=1;
                    }
                    else flag=10;
                    
                    puts(server_reply);
                    sleep(1);
                    system("clear");
                    
                    
                    //Dovrò effettuare l'accesso al server
                    break;
                case 2:
                    do{
                        
                        if( send(sock , "2" , sizeof("2") , 0) < 0){
                            puts("Send failed");
                            break;
                        }
                        strcpy(server_reply," ");
                        if( recv(sock , server_reply , sizeof(server_reply) , 0) < 0){
                            puts("Errore ricezione messaggio");
                            break;
                        }
                        puts(server_reply);
                        scanf("%s",username);
                        if(send(sock,username,strlen(username),0) < 0){
                            puts("Errore comunicazione server");
                            break;
                        }
                        if(recv(sock,server_reply,sizeof(server_reply),0) < 0){
                            puts("Errore ricezione messaggio");
                            break;
                        }
                        if(strcmp(server_reply,"Username gia presente")==0){
                            puts("Username gia presente, ripeti selezione\n");
                            flag=2;
                        }
                        else flag=3;
                    }while(flag!=3);
                    puts(server_reply);
                    pw=getpass("");
                    if(send(sock,pw,strlen(pw), 0) < 0 ){
                        puts("Errore comunicazione server");
                        break;
                    }
                    
                    if(recv(sock,server_reply2,sizeof(server_reply2),0) < 0){
                        puts("Errore ricezione messaggio");
                        break;
                    }
                    puts(server_reply2);
                    sleep(2);
                    
                    system("clear");
                    break;
                    
                default:
                    //Qualcosa non ha funzionato
                    break;
            }
        }while(flag!=10);
        
        //Comunicazione con il Server
        
        if( send(sock , "3" , sizeof("3") , 0) < 0){
            puts("Send failed");
        }
        if( recv(sock , server_reply ,sizeof(server_reply) , 0) < 0){
            puts("Errore ricezione messaggio");
        }
        if(send(sock,username,strlen(username),0) < 0){
            puts("Errore comunicazione server");
        }
        
        strcpy(server_reply," ");
        if(recv(sock,server_reply2,sizeof(server_reply2),0) < 0 ){
            puts("Errore ricezione messaggio");
            
        }  
        
        
        
        if(strcmp(server_reply2,"1")==0){
            //Deve effettuare il primo inserimento.
            
            
            do{
                puts("\n\nInserisci l'esercito di appartenenza.\n1 - BLU\n2 - ROSSO\n");
                scanf("%s",comando);
                if(strcmp(comando,"1")==0){
                    if(send(sock,"1",sizeof("1"),0) < 0){
                        puts("Send Failed");
                        break;
                    }
                    flag=1;
                }
                else if(strcmp(comando,"2")==0){
                    if(send(sock,"2",sizeof("2"),0) < 0){
                        puts("Send Failed");
                        break;
                    }
                    flag=1;
                }
                else puts("\nInserisci un valore valido.\n");
                //voglio vedere se ci sono almeno un gioatore per squadra 
                puts("Attendere altri giocatori ... \n");
                
                if(flag==1){
                    //Mi stampo la matrice che ricevo dal server.
                    strcpy(server_reply," ");
                    
                    if(recv(sock,server_reply,sizeof(server_reply),0) < 0){
                        puts("Errore ricezione messaggio");
                        break;
                    }
                    puts("\n");
                    puts(server_reply);
                    strcpy(server_reply," ");
                    if(send(sock,"1",sizeof("1"),0) < 0){
                        puts("Send Failed");
                        break;
                    }
                    if(recv(sock,server_reply,sizeof(server_reply),0) < 0){
                        puts("Errore ricezione messaggio");
                        break;
                    }
                    puts("L'inserimento di una riga o colonna fuori dalla dimensione della \nmatrice comporta il posizionamento 0,0\n");
                    puts("\n");
                    puts(server_reply);
                    scanf("%s",posizione);
                    if(send(sock,posizione,strlen(posizione), 0) <0){
                        puts("Send Failed");
                        break;
                    }  
                    
                    if(recv(sock,server_reply3,sizeof(server_reply3),0) < 0) {
                        puts("Errore ricezione messaggio");
                        break;
                    }
                    
                    puts(server_reply3);
                    scanf("%s",posizione);
                    if(send(sock,posizione,strlen(posizione),0) <0){
                        puts("Sendi Failed");
                        break;
                    }
                    flag=2;
                }
            }while(flag!=2);
            
            puts("Inizializzazione Giocatore Completata\nAdesso potrai prendere parte alla partita");
        }
        sleep(1);
        while(flag!=5){
            
            system("clear");
            //Stampo il mio Logged Menu
            loggedmenu();
            while(1){
                puts("\n");
                //Ricavo la scelta dell'user
                scanf("%s",comando);
                if(flag==1) strcpy(comando,"2");
                scelta=atoi(comando);
                
                if(scelta > 0 && scelta < 7) break;
                puts("Inserisci un operazione valida.");
            }
            n=snprintf(comando,2,"%d",scelta+3);
            
            
            if(send(sock,comando,strlen(comando),0) <0){
                puts("Send Failed");
                break;
            }
            puts("\n");
            
            
            
            switch(scelta){
                case 1:
                    if(recv(sock,server_reply,sizeof(server_reply),0) < 0) {
                        puts("Errore ricezione messaggio");
                        break;
                    }
                    puts("Stai effettuando lo spostamento");
                    puts(server_reply);
                    
                    while(1){
                        puts("\n");
                        scanf("%s",comando);
                        scelta=atoi(comando);
                        if(scelta > 0 && scelta < 5) break;
                        puts("Selezione non valida, Inserisci un valore valido");
                    }
                    //Mando Al server lo spostamento.
                    if(send(sock,comando,strlen(comando),0) < 0){
                        puts("Send Failed");
                        break;
                    }
                    if(recv(sock,server_reply,sizeof(server_reply),0) < 0){
                        puts("Errore ricezione messaggio");
                        break;
                    }
                    //Mando al server l'username
                    if(send(sock,username,strlen(username),0) < 0) {
                        puts("Send Failed");
                        break;
                    }
                    if(recv(sock,server_reply,sizeof(server_reply),0) < 0){
                        puts("Errore ricezione messaggio");
                        break;
                    }
                    //Mando al server la password
                    if(send(sock,password,strlen(password),0) < 0){
                        puts("Send Failed");
                        break;
                    }
                    if(recv(sock,server_reply,sizeof(server_reply),0) < 0){
                        puts("Errore ricezione messaggio");
                        break;
                    }
                    if(strcmp(server_reply,"1")==0){          
                        puts("hai vinto");
                        scelta=2;        
                        while(getchar()!='\n');
                        ch=getchar();
                        
                        flag=5;
                    }
                    else {
                        puts(server_reply);
                        puts("Clicca invio per continuare");
                        //Libero il buffer
                        while(getchar()!='\n');        
                        ch=getchar();
                    }
                    break;
                    case 2:
                        
                        strcpy(server_reply2," ");
                        if(recv(sock,server_reply2,sizeof(server_reply2),0) < 0) {
                            puts("Errore ricezione messaggio");
                            break;
                        }
                        puts(server_reply2);
                        if(send(sock,username,strlen(username),0) < 0) {
                            puts("Send Failed");
                            break;
                        }
                        
                        
                        
                        flag=5;
                        
                        
                        break;
                        
                        
                        
                    case 3:
                        
                        strcpy(server_reply3," ");
                        if(recv(sock,server_reply3,sizeof(server_reply3),0)<0){
                            puts("Errore ricezione messaggio");
                            break;
                        }
                        puts(server_reply3);
                        puts("Clicca invio per continuare");
                        //Libero il buffer
                        while(getchar()!='\n');        
                        ch=getchar();
                        break;
                        
                    case 4:
                        
                        if(recv(sock,server_reply4,sizeof(server_reply4),0) < 0){
                            puts("Errore ricezione messaggio");
                            break;
                        }
                        if(send(sock,username,strlen(username),0) < 0){
                            puts("Send Failed");
                            break;
                        }
                        
                        if(recv(sock,server_reply4,sizeof(server_reply4),0) < 0){
                            puts("Errore ricezione messaggio");
                            break;
                        }
                        puts(server_reply4);
                        puts("Clicca invio per continuare");
                        //Libero il buffer
                        while(getchar()!='\n');        
                        ch=getchar();  
                        
                        break;
                        
                    case 5:
                        if(recv(sock,server_reply5,10,0) < 0){
                            puts("Errore ricezione messaggio");
                            break;
                        }
                        if(send(sock,username,strlen(username),0) < 0){
                            puts("Send Failed");
                            break;
                        }
                        if(recv(sock,server_reply5,sizeof(server_reply5),0) < 0){
                            puts("Errore ricezione messaggio");
                            break;
                        }
                        puts(server_reply5);
                        puts("Clicca invio per continuare");
                        //Libero il buffer
                        while(getchar()!='\n');        
                        ch=getchar();  
                        
                        break;
                        
                    case 6:
                        if(recv(sock,server_reply2,sizeof(server_reply2),0) < 0) {
                            puts("Errore ricezione messaggio");
                            break;
                        }
                        //Mando al server l'username
                        if(send(sock,username,strlen(username),0) < 0) {
                            puts("Send Failed");
                            break;
                        }
                        strcpy(server_reply2," ");
                        //Ricevo La mia matrice
                        if(recv(sock,server_reply2,sizeof(server_reply2),0) < 0){
                            puts("Errore ricezione messaggio");
                            break;
                        }
                        
                        puts(server_reply2);
                        puts("Clicca invio per continuare");
                        //Libero il buffer
                        while(getchar()!='\n');        
                        ch=getchar();
                        
                        break;
                        
                    default:
                        strcpy(server_reply2," ");
                        if(recv(sock,server_reply3,sizeof(server_reply3),0) < 0){
                            puts("Errore ricezione messaggio");
                            break;
                        }
                        puts(server_reply3);
                        break;
            }
            
            
            
            
        }
        
        puts("Torno indietro");
    }while(1);
    
    
}







int main(int argc , char *argv[])
{
    int sock,sock2;
    struct sockaddr_in server,server2;
    pthread_t figlio;
    char mynumb[3];
    int id;
    
    memset((void*)&server,0,sizeof(server)); //Inizializzazione
    //Creo il socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        perror("socket");
        return 1;
    }
    sock2 = socket(PF_INET , SOCK_STREAM , 0);
    if (sock2 == -1)
    {
        perror("socket");
        return 1;
    }
    if(argv[2]!=NULL){
        server.sin_addr.s_addr= inet_addr(argv[2]);
    }
    else server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons( atoi(argv[1]));
    
    
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connessione fallita");
        return 1;
    }
    
    
    
    connettiti(sock);
    
    return 0;
}
