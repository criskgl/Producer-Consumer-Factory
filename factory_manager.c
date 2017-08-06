/*
 *
 * factory_manager.c
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stddef.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

//Footprints
void tostring(char str[], int num);

int main (int argc, const char * argv[] ){

	if(argc < 2 || argc > 2){
		fprintf(stderr, "[ERROR][factory manager]Invalid file.\n");
		exit(-1);
	}

	FILE *myFile;

	int maxSizeArray = 0;
	int maxcintas = 0;

	//OPEN file
	if((myFile = fopen(argv[1], "r")) == NULL){
		fprintf(stderr, "[ERROR][factory manager]Invalid file.\n");
		exit(-1);
	}

	//READ file
	//Read maximum number of cintas
	if(fscanf(myFile, "%d", &maxcintas) == 0){
		fprintf(stderr, "[ERROR][factory manager]Invalid file.\n");
		exit(-1);
	}

	//error control: MaxCintas > 0
	if(maxcintas <= 0){
		fprintf(stderr, "[ERROR][factory manager]Invalid file.\n");
		exit(-1);
	}

	//Initialize array correspondent to maximum number of cintas(3 elements per cinta)
	maxSizeArray = 3*maxcintas;
	int data[maxSizeArray];
	int contadorElems = 0;
	int returnScan = 1;

	//scan file to find numbers
	while((returnScan = fscanf(myFile, "%i", &data[contadorElems])) != EOF && contadorElems < maxSizeArray){
		if(returnScan == 0){
			fprintf(stderr, "[ERROR][factory manager]Invalid file.\n");
			exit(-1);
		} 
		contadorElems++;
	}

	int numberPM = contadorElems/3;

	//error control: check for complete parameters in each cinta
	if(contadorElems % 3 != 0 || contadorElems == 0){
		fprintf(stderr, "[ERROR][factory manager]Invalid file.\n");
		exit(-1);
	}

	//CLOSE file
	fclose(myFile);

	//error control: check for negative values in triadas
	for (int i = 0; i < contadorElems; i++)
	{
		if(data[i] <= 0 ){
			fprintf(stderr, "[ERROR][factory manager]Invalid file.\n");
			exit(-1);
		}
	}

	int myids[numberPM];
	int i = 0;
	int j = 0;

	//save triada's id's
	while(j < numberPM){
		//assign ids to array
		myids[j] = data[i];
		//step
		j++;
		i = i+3;
	}

	//error control: check for repeated cinta id´s as many times as maximum number of REAL cintas
	for (int i = 0; i < numberPM; i++)
	{
	   for (int j = i + 1; j < numberPM; j++){
		   	if (myids[i] == myids[j])
		    {
		       fprintf(stderr, "[ERROR][factory manager]Invalid file.\n");
		       exit(-1);
			}
		} 
	}

	//CREATE AS MANY SEMAPHORES AS PROCESS_MANAGERS
	char semNames[numberPM][10];
	sem_t *idSems[numberPM];
	sem_t semts[numberPM];
	
	for(int i = 0; i < numberPM; i++){

		char nameSem[10];
		if((sprintf(nameSem, "/%d", i))<0){
			fprintf(stderr, "[ERROR][factory manager]Error while naming semaphore.\n");
			exit (-1);
		}
		strcat(nameSem, "\0");
		const char* ptrNameSem = nameSem;
		strcpy(semNames[i], ptrNameSem);

		if((idSems[i] = sem_open(ptrNameSem, O_CREAT, 0777, 0)) == SEM_FAILED){
			fprintf(stderr, "[ERROR][factory manager]Error while opening semaphore.\n");
			exit (-1);
		}
		semts[i] = *idSems[i];
	}

	//CREATE CHILD PROCESS (PROCESS MANAGER) acording to numberPM
	pid_t pid;
	pid_t childPids[numberPM];
	int k;

	for(k = 0; k < numberPM; k++) {
		pid = fork();
		if(pid < 0) {
			fprintf(stderr, "[ERROR][factory_manager]Error while forking process_manager.\n");
	    	exit(-1);
		} else if (pid == 0) {//child
	    	printf("[OK][factory_manager] Process_manager with id %d has been created.\n", data[3*k]);
	    	
	    	char arg1s[10], arg3s[10], arg4s[10]; //args2s will be directly passed as a string.

	    	sprintf(arg1s, "%d", data[3*k]);
	    	sprintf(arg3s, "%d", data[3*k+1]);
	    	sprintf(arg4s, "%d", data[3*k+2]);

	   		execlp("./process", "./process", arg1s, semNames[k], arg3s, arg4s, (char *)NULL);
	   		fprintf(stderr, "[ERROR][factory_manager] Process_manager with id %d has finished with errors.\n", data[3*k]);
	    	exit(-1);
		}else{//Parent
			childPids[k] = pid;
		}
	}
	
	if(k == numberPM){
	//ACTIVATE EACH PM
	for(int j = 0; j < numberPM; j++){
   		int status;
   		//SEM_POST-->Activate each waiting PM
   		idSems[j] = sem_open(semNames[j], O_CREAT, NULL, 0);
   		if(idSems[j] == SEM_FAILED){
			fprintf(stderr, "[ERROR][factory_manager] Could not read semaphore.\n");
			exit (-1);
   		}
		if((sem_post(idSems[j]) == -1)){
			fprintf(stderr, "[ERROR][factory_manager] Could not do V operation on semaphore.\n");
			exit (-1);
		}
		while(waitpid(childPids[j], &status, WNOHANG) != childPids[j]);
		printf("[OK][factory_manager] Process_manager with id %d has finished.\n",data[3*j]);
	}

	//FREE MEMORY

	for(int i = 0; i < numberPM; i++){
		const char* ptrNameSem = (const char*)semNames[i];
		if((sem_unlink(ptrNameSem)) == -1){
			fprintf(stderr, "[ERROR][factory_manager] Could not unlink semaphore.\n");
			exit (-1);
		}
	}

	for(int i = 0; i < numberPM; i++){
		if((sem_close(idSems[i])) == -1){
			fprintf(stderr, "[ERROR][factory_manager] Could not close semaphore.\n");
			exit (-1);
		}
	}

	printf("\n[OK][factory_manager] Finishing.\n");
	return 0;
	}
	
}

void tostring(char str[], int num)
{
    int i, resto, len = 0, n;
 
    n = num;
    while (n != 0)
    {
        len++;
        n /= 10;
    }
    for (i = 0; i < len; i++)
    {
        resto = num % 10;
        num = num / 10;
        str[len - (i + 1)] = resto + '0';
    }
    str[len] = '\0';
}