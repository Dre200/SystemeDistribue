
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MY_ADDR "127.0.0.1"
#define MY_PORT 56789
#define LISTEN_BACKLOG 50

int main(int argc, char *argv[]){
   FILE* fichier = NULL;
   char line[1];
   int sfd; 
   int cfd; 
   struct sockaddr_in my_addr; 
   struct sockaddr_in peer_addr; 

   socklen_t peer_addr_size;
   pid_t child;

   char buffer[10]; 

   sfd = socket(AF_INET, SOCK_STREAM, 0); 
   if (sfd < 0){ 
      printf("Le SE n'a pas pu créer la socket %d\n", sfd);
      exit(-1);
   }

   /* La structure est remise à 0 car elle doit servir à stocker
    * l'identité du serveur*/
   memset(&my_addr, 0, sizeof(struct sockaddr_in));
   
    /* donne une identite a la socket. */
   my_addr.sin_family = AF_INET;
   my_addr.sin_port = htons (MY_PORT);
   inet_aton(MY_ADDR, (struct in_addr *)&my_addr.sin_addr.s_addr);
  
   
   if (bind(sfd, (struct sockaddr *) &my_addr,
                   sizeof(struct sockaddr_in)) < 0){
       printf("bind error\n"); // l'association a echouée
       exit(-1);
  }

 
   if (listen(sfd, LISTEN_BACKLOG) < -1)
               perror("listen\n");

   // on se bloque en attendant les connexion des client
   peer_addr_size = sizeof(struct sockaddr_in);
   while(1){
         cfd = accept(sfd, (struct sockaddr *) &peer_addr,
                   &peer_addr_size);  
        if (cfd < 0){
            perror("accept\n");
            exit(-1); 
        }
        /* Nous créons un fils pour gérer ce client */
        child = fork();
        if(child < 0){ // le fils n'a pas pu être créé
           perror("errreur de création du fils\n");
        }
        if(child==0){
             printf("identité du client %d\n", peer_addr.sin_port);
             
            
             read(cfd,buffer,sizeof(buffer));
   
             fichier = fopen(buffer,"w");
             if(fichier!=NULL){
                 printf("Fichier ouvert");
             while (read(cfd,line, sizeof(line)))
				fwrite(line,sizeof(line),1,fichier);
             printf("Fin de lecture\n");
             close(sfd);
            
             break;
        }
	}

        else{
          /*Dans le père: le père attent un autre client*/
          close(cfd);
        }
  }
}
