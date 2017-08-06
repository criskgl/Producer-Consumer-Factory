/*
 *
 * process_manager.c
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stddef.h>
#include <pthread.h>
#include "queue.h"
#include <semaphore.h>

#define NUM_THREADS 2
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int idCinta;
int maxBeltSize;
int toGenerate;

void *Consumer(){

   struct element *elemGotptr;
   struct element elemGot;

   while(1){ 
      if(queue_empty() == 0){//queue has something...
         pthread_mutex_lock(&mutex);//secure queue
         elemGotptr = queue_get();
         elemGot = *elemGotptr;

         if(elemGot.last == 1){//check  if element got is last-->terminate
            pthread_exit(NULL);
         }
         pthread_mutex_unlock(&mutex);//release queue
      }
   }
}

void *Producer(){
   struct element elem;
   int i = 0;
   
   while(1){
      if(queue_empty() == 1){//queue empty-->put elements in queue
         pthread_mutex_lock(&mutex);//secure queue
         elem.num_edition = i;
         elem.id_belt = idCinta;
         if(i == toGenerate-1){
            elem.last = 1;
         }
         if(queue_put(&elem) == -1){
            int valueExit = -1;
            pthread_exit(&valueExit);
         }

         i++;
         pthread_mutex_unlock(&mutex);//release queue
      }
      if(i == toGenerate){
         pthread_exit(NULL);
      }
   }
}


int main (int argc, const char * argv[] ){
   
   if(argc > 5 || argc < 5){//error control passing arguments
      fprintf(stderr, "[ERROR][process_manager] Arguments not valid.\n");
      exit(-1);
   }

   //convert (only) string arguments to int
   char *ptr;
   idCinta = strtol(argv[1], &ptr,10);
   maxBeltSize= strtol(argv[3], &ptr,10);
   toGenerate = strtol(argv[4], &ptr,10);

   //OPEN SEMAPHORE
   sem_t *idSemaphore;
   idSemaphore = sem_open(argv[2],O_CREAT, NULL, 0);

   if(idSemaphore == SEM_FAILED){
      fprintf(stderr,"[ERROR][process_manager] Process manager could not open semaphore\n");
      exit(-1);
   }

   //STOP PM until sempost
   printf("[OK][process_manager] Process_manager with id: %d waiting to produce %d elements.\n",idCinta, toGenerate);
   if(sem_wait(idSemaphore) == -1){
      fprintf(stderr,"[ERROR][process_manager] Process manager could not do P operation on semaphore\n");
      exit(-1);
   }
      //THIS CODE WILL EXECUTE WHEN SEM_POST IS DONE IN FM
      //QUEUE
      queue_init(maxBeltSize);
      
      //INITIALIZE MUTEX
      pthread_mutex_init(&mutex, NULL);
      
      //ACTIVATE CONSUMER AND PRODUCER
      pthread_t idConsumer, idProducer;

      printf("[OK][process_manager] Belt with id: %d has been created with a maximum of %d elements.\n",idCinta, maxBeltSize);
      pthread_create(&idProducer, NULL, Producer, NULL);
      pthread_create(&idConsumer, NULL, Consumer, NULL);
      
      if((pthread_join(idProducer, NULL)) != 0 ){;
         fprintf(stderr, "[ERROR][process_manager] There was an error executing process_manager with id: %d.\n", idCinta);
      }   
      //printf("Producer has finished\n");
      if((pthread_join(idConsumer, NULL)) != 0){
         fprintf(stderr, "[ERROR][process_manager] There was an error executing process_manager with id: %d.\n", idCinta);
      }
      //printf("Consumer has finished\n");
      printf("[OK][process_manager] Process_manager with id: %d has produced %d elements.\n", idCinta, toGenerate);

      queue_destroy();

      sem_close(idSemaphore);

      exit (0);
}