#include <stdlib.h>
#include <stdio.h>
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <sys/sem.h>
#include <errno.h>
#include <sys/types.h>

int main(int argc, char* argv[])
{
	int dimension=atoi(argv[1]), nbElement;
	int i=atoi(argv[2]);
	int j=atoi(argv[3]);
	int k=0,l=0;
	int * tabI=NULL, *tabJ=NULL; 
    int *matrice=NULL;
	
	
	int shmidm;
    key_t keym;
    
    nbElement=dimension*dimension;
    
    printf("Processus pour le calcul de C(%d,%d): %d\n",i,j, getpid());
	
	tabI = malloc(dimension * sizeof(int));
	 if (tabI == NULL) {
        printf(" Allocation de mémoire impossible\n");
        exit(EXIT_FAILURE);
    }
   tabJ= malloc(dimension * sizeof(int));
	
	if(tabJ == NULL) {
        printf("Allocation de mémoire impossible\n");
        free(tabI);
        exit(EXIT_FAILURE);
    }
    
    keym=ftok("shmMatrice", 19);
	shmidm= shmget(keym, 2 * nbElement * sizeof(int),0666);
	matrice=(int*) shmat(shmidm,(void*)0,0);
	
	if (matrice == NULL) {
        printf("Processus C(%d,%d): Lecture de la mémoire partagée impossible\n", i, j);
        free(tabI);
        free(tabJ);
        exit(EXIT_FAILURE);
    }
    
    //Copie des donnees de la matrice
    
    for(k=0;k<dimension;k++){
		tabI[k] = matrice[i*dimension + k];
        tabJ[k] = matrice[j+dimension * k];
     }   
        
     
     
     //Calcul du Cij
      matrice[i*dimension + j + nbElement] = 0;
    for (k= 0;k<dimension;k++) {
        matrice[i*dimension+ j +nbElement] += tabI[k] * tabJ[k];  
    }
	
    shmdt(matrice);

    
    free(tabI);
    free(tabJ);
	
	
	return 0;
}
