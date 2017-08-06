#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include "queue.h"

int length();
void printList();


//NodeQueue
typedef struct Node
{
   struct element elem;
   struct Node *next;
   struct Node *prev;
}node_t;

struct Node *head;
struct Node *current;
int queueSize;
int currentSize;

//To create a queue
int queue_init(int size){

   head = NULL;

   current = NULL;

   queueSize = size;

   return 0;
}

// To Enqueue an element
int queue_put(struct element* x) {

   //create a new node
   node_t *newnode = (node_t*) malloc(sizeof(node_t));
   if(newnode == NULL){
      fprintf(stderr, "[ERROR][queue] There was an error while using queue with id: %d.\n", x->id_belt);
      return (-1);
   }
   newnode->elem = *x;

   if(currentSize == queueSize){
      return (-1);
   }

   if (queue_empty() == 1) {
      head = newnode;
      head->next = head;
      head->prev = head;
   } else {
      //point it to old first node
      newnode->next = head;
      newnode->prev = head->prev;
      head->prev = newnode;        
      //point first to new first node
      head = newnode;

   }
   currentSize++;
   printf("[OK][queue] Introduced element with id: %d in belt %d.\n",newnode->elem.num_edition, newnode->elem.id_belt);
   return 0;    
}


// To Dequeue an element.
struct element* queue_get(void) {
   
   //save reference to first link
   struct Node *tempLink;
   tempLink = head->prev;
   if(head->next == head) {//one element?
      struct element elem = head->elem;
      struct element *returnElement;
      returnElement = &elem;
      head = NULL;
      currentSize--;
      printf("[OK][queue] Obtained element with id: %d in belt %d.\n",returnElement->num_edition, returnElement->id_belt);
      return returnElement;
   }//more than 1 element

   //mark next to first link as first 
   head->prev->prev->next = head;
   head->prev = head->prev->prev;
   currentSize--;
   //return the deleted link
   printf("[OK][queue] Obtained element with id: %d in belt %d.\n",tempLink->elem.num_edition, tempLink->elem.id_belt);
   return &(tempLink->elem);
}


//To check queue state
int queue_empty(void){
   
   if(head == NULL){
      return 1;
   }else{
      return 0;
   }
   
}

int queue_full(void){

   if(length() == queueSize){
      return 1;
   }else{
      return 0;
   }
}

//To destroy the queue and free the resources
int queue_destroy(void){

   current = head->next;
   node_t *auxnext;

   while(current != head){
      auxnext = current->next;
      free(current); 
      current = auxnext;
   }
   free(head);
   
   return 0;
}

int length() {
   int length = 0;

   //if list is empty
   if(head == NULL) {
      return 0;
   }

   current = head->next;

   while(current != head) {
      length++;
      current = current->next;   
   }
   
   return length;
}

//display the list
void printList() {

   int current = 0;
   struct Node *ptr = head;
   
   //printf("current size:%d \n", currentSize);
   printf("\n[ ");
   
   //printf("%d", ptr->elem.num_edition);
   //start from the beginning
   if(head != NULL) {
   
      while(current < currentSize) {     
         printf("(%d) ",ptr->elem.num_edition);
         ptr = ptr->next;
         current++;
      }
   }
   
   printf(" ]\n");
}