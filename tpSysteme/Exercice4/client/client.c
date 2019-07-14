#include <sys/socket.h>
#include <netinet/in.h>
 #include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>


int main(int argc, char *argv[]){
   int cfd;
   int success; 
   int port;
   size_t nbytes = 0;
   char line[1];
 
   
   
   FILE *fichier = NULL; 
 

   struct sockaddr_in srv_addr;  

   if(argc!=4){
     printf("USAGE:./client <serveur_ip_addr> <serveur_port> <fichier a envoyer>\n");
     exit(-1);
   }

   cfd = socket(AF_INET, SOCK_STREAM, 0); 
   if (cfd < 0){ 
      printf("Le SE n'a pas pu créer la socket %d\n", cfd);
      exit(-1);
   }
  
   port = atoi(argv[2]);

   srv_addr.sin_family = AF_INET;
   srv_addr.sin_port = htons (port);
   inet_aton(argv[1], (struct in_addr *)&srv_addr.sin_addr.s_addr);

   success = connect(cfd, (struct sockaddr *) &srv_addr,
                 sizeof(struct sockaddr_in));
   if(success < 0){ // la connexion a echouée
      printf("Impossible de se connecter au serveur %s:%d error %d\n", 
               argv[1], port, success);
      exit(-1);
   }
   
     
      send(cfd,argv[3], sizeof(argv[3]), 0);
      sleep(7);
      
      fichier = fopen(argv[3],"r");
      while (fread(line,sizeof(line), 1,fichier),!feof(fichier)) {
		  
			 send(cfd,line, sizeof(line), 0);
			 
			 
	  }
   close(cfd);
   fclose(fichier);
   return(0);
}

