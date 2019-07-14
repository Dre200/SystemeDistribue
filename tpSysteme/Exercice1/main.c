/* Ce programme lance 3 proccessus fils et affiche leur pid
11/07/2019
 * Auteur: DJESSOU Registe regisdjessou2@gmail.com*/
 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
int main()
{  pid_t pid1, pid2, pid3;
	int status;
	pid1 = fork();
    if(pid1 < 0){
          perror("Erreur de création du processus\n");
          exit(EXIT_FAILURE);
    }
    
    if(pid1 == 0){
        printf("Le pid du processus fils 1 est:%d\n",getpid());
    }
     else{
        
        pid2 = fork();
        if(pid2 < 0){
          perror("Erreur de création du second processus\n");
          pid1 = wait(&status);
          exit(EXIT_FAILURE);
        }
        if(pid2 == 0){
            printf("Le pid du processus fils 2 est:%d\n",getpid());
        }
         else{
            pid3 = fork();
            if(pid3 < 0){
              perror("Erreur de création du troisieme processus\n");
              pid1 = wait(&status);
              pid2 = wait(&status);
              exit(EXIT_FAILURE);
            }
             if(pid3 == 0){
                 printf("Le pid du processus fils 3 est:%d\n",getpid());
            }

            else{
                pid1 = wait(&status);
                pid2 = wait(&status);
                pid3 = wait(&status);
            }
        }

        
    } 
	
}
