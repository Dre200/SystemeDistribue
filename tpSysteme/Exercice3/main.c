#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <sys/sem.h>
#include <errno.h>

#define SPACE ' '

int nbChiffre(int nb)
{
    int i;
    for (i = (nb <= 0) ? 1 : 0; nb != 0; i++) nb /= 10;
    return i;   
}
int printcol(const int colWidth, const int nb)
{
    const int strWidth = nbChiffre(nb);
    int nbrSpace;

    if (colWidth > 0 && strWidth <= colWidth){
        for (nbrSpace = colWidth - strWidth; nbrSpace > 0; nbrSpace--)
            putchar(SPACE);
        printf("%d", nb);
        return 1;
    }
    else return 0;
}

int main(int argc, char* argv[])
{   FILE* fichier=NULL;
	int* matrice = NULL;
	pid_t** pid_tTab = NULL;//Tableau de pid
	int dimension=0,nbElement=0,tColonne = 0, status;
	
	int i=0,j=0,k=0,l=0;
	
	
	char* cO=NULL, *cI=NULL, *cJ = NULL;
	
	int shmidm;
    key_t keym;
	
	if(argc!=2)
	{
		printf("USAGE <nomfichier>");
		exit(EXIT_FAILURE);
	}
	
	fichier=fopen(argv[1],"r");
	
	if(fichier==NULL){
			printf("Ce fichier n'existe pas");
			exit(EXIT_FAILURE);
	}
	//Recuperation de la dimension du tableau
	fscanf(fichier,"%d",&dimension);
	
	if(dimension<2)
	{
		printf("Choississez une matrice dont l'ordre est superieur a 2");
		fclose(fichier);
        exit(EXIT_FAILURE);
	}
	
	nbElement=dimension*dimension;
	//Creation de la matrice en memoire partagee
	keym=ftok("shmMatrice", 19);
	shmidm= shmget(keym, 2*nbElement *sizeof(int),0666|IPC_CREAT);
	matrice=(int*) shmat(shmidm,(void*)0,0);
	
	if (matrice == NULL) {
        printf("Allocation de mémoire impossible !\n");
        fclose(fichier);
        exit(EXIT_FAILURE);
    }
	
	
	 //Recuperation de la matrice
	
	for(i=0;i<dimension;i++){
		for(j=0;j<dimension;j++)
		{
			fscanf(fichier,"%d ",&matrice[i*dimension+j]);
			 
			 if(tColonne<nbChiffre(matrice[i*dimension +j]))
			
				tColonne=nbChiffre(matrice[i*dimension+j]);
			//Verification qu'on recupere une matrice carree d'ordre la dimension du tableau
		
		  if(feof(fichier)&& j!=dimension-1){
			  
			  fclose(fichier);
              shmdt(matrice); 
              shmctl(shmidm, IPC_RMID, NULL);
              printf("Matrice carrée  incomplète !\n");
              exit(EXIT_FAILURE);
		  
		  }
		}
		
	}
	fclose(fichier);
	printf("La matrice a bien ete lue\n");
	 
	//Affichage de la matrice pour le calcul
	
	for(i=0; i<dimension;i++){
		
		putchar('+');
		for(j=0;j<dimension;j++){
			for(k=0;k<tColonne;k++)
				putchar('-');
			putchar('+');
		}
		printf("\n|");
		
		for(j=0;j<dimension;j++){
			printcol(tColonne, matrice[i*dimension + j]);
			putchar('|');	
		}
		 printf("\n"); 
	}
	putchar('+');
	
	for(i=0;i<dimension;i++){
		for(j=0;j<tColonne;j++)
			putchar('-');
		putchar('+');
	}
		
	printf("\n\n");	
	
	//Creation des processus
	pid_tTab=malloc(dimension*sizeof(pid_t*));	
	
	if(pid_tTab==NULL){
		
		 printf("Allocation de mémoire impossible !\n");
        
        shmdt(matrice); 
        shmctl(shmidm, IPC_RMID, NULL);
        exit(EXIT_FAILURE);
	}
	
	for(i=0;i<dimension;i++){
		 pid_tTab[i] = malloc(dimension * sizeof(pid_t));
		
		if(pid_tTab[i] == NULL){
			printf("Allocation de mémoire impossible !\n");
            
            for (j = 0; j < i; j++)
                free(pid_tTab[j]);
                
             free(pid_tTab);
		     shmdt(matrice); 
			 shmctl(shmidm, IPC_RMID, NULL);
            
			exit(EXIT_FAILURE);
			
		}
	}
	//Transmission des ij  aux processus 
	cO=malloc((1+nbChiffre(dimension))* sizeof(char));
	if (cO == NULL) {
        printf("Allocation de mémoire impossible !\n");
            for (i = 0; i <dimension; i++)
                free(pid_tTab[i]);
           
        free(pid_tTab);
        
        shmdt(matrice); 
        shmctl(shmidm, IPC_RMID, NULL);
        
        exit(EXIT_FAILURE);
    } 
    
    sprintf(cO,"%d",dimension);
    argv[1]=cO;
    
    cI=malloc((1+nbChiffre(dimension))* sizeof(char));
	if (cI == NULL) {
        printf("Allocation de mémoire impossible !\n");
            for (i = 0; i <dimension; i++)
                free(pid_tTab[i]);
         
        free(cO);      
        free(pid_tTab);
        
        shmdt(matrice); 
        shmctl(shmidm, IPC_RMID, NULL);
        
        exit(EXIT_FAILURE);
    }
     
    cJ=malloc((1+nbChiffre(dimension))* sizeof(char));
	if (cJ == NULL) {
        printf("Allocation de mémoire impossible !\n");
            for (i = 0; i < dimension; i++)
                free(pid_tTab[i]);
         
        free(cO);
        free(cI); 
             
        free(pid_tTab);
        
        shmdt(matrice); 
        shmctl(shmidm, IPC_RMID, NULL);
        
        exit(EXIT_FAILURE);
    }	
    
    //Creation des processus et passage en parametres
     
     for(i=0;i<dimension;i++){
		 for(j=0;j<dimension;j++){
			 sprintf(cI,"%d",i);
			 argv[2]=cI;
			 sprintf(cJ, "%d", j);
             argv[3] = cJ;
             
             pid_tTab[i][j]=fork();
             
             if(pid_tTab[i][j]<0){
				 printf("Processus C(%d,%d) non cree",i,j);
			 
			 for(k=0;k<=i;k++){
				for(l=0;l<j;l++)
					wait(&status);
			 } 
			 for (k=0; k<dimension; k++)
                    free(pid_tTab[i]);
                        
                free(cO);
                free(cI);
                free(cJ);
                free(pid_tTab);
                
                shmdt(matrice); 
                
                shmctl(shmidm, IPC_RMID, NULL); 
                
                exit(EXIT_FAILURE);	 
			 }
			 
			 if (pid_tTab[i][j] == 0) {
                execv("./Cij", argv);
            }
            sleep(2);
			 
		 }
	 }
	 
	 //Fin des processus et des programmes
	 
     if(getpid()>0){
		 tColonne=0;
		 
		 for(i=0;i<nbElement;i++){
			 if(tColonne<nbChiffre(matrice[i+nbElement]))
				tColonne=nbChiffre(matrice[i+nbElement]);
		 }
		 
	//Affichage du resultat
	for(i=0; i<dimension;i++){
		
		putchar('+');
		for(j=0;j<dimension;j++){
			for(k=0;k<tColonne;k++)
				putchar('-');
			putchar('+');
		}
		printf("\n|");
		
		for(j=0;j<dimension;j++){
			printcol(tColonne, matrice[i*dimension + j +nbElement]);
			putchar('|');	
		}
		 printf("\n"); 
	}
	putchar('+');
	
	for(i=0;i<dimension;i++){
		for(j=0;j<tColonne;j++)
			putchar('-');
		putchar('+');
	}
	printf("\n\n");	
	
		free(cO);
		free(cI);
        free(cJ);
        
        for(i=0;i<dimension;i++){
			for(j=0;j<dimension;j++){
				wait(&status);
				printf("Arrêt du processus %d %d\n", status, pid_tTab[i][j]);
		}	
			free(pid_tTab[i]);	
			
		}
        
	     free(pid_tTab);
	     free(matrice);
	     shmctl(shmidm, IPC_RMID, NULL);
		 putchar('\n');
		}
    
		return 0;
	}
	 
	
	 
	
	
	

