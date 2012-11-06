#ifndef __HASH_H__

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>

/*THIS IS THE HEADER FILE FOR SEBASTIAN AND NICKIE'S THINGY WINGY DINGY DINGY*/

struct node {
	char name[128];
    struct node *next;
};
  

typedef struct {
	// Array used to map alphabetical characters to
	struct node** table;
	int tablesize;
	pthread_mutex_t * listlocks;
	pthread_mutex_t hashlock;
	
} hashtable_t;


//Used to get hash value (the location in our char* table)
int hashFunction(const char* string, int tablelen);

// create a new hashtable; parameter is a size hint
hashtable_t *hashtable_new(int sizehint);

// free anything allocated by the hashtable library
void hashtable_free(hashtable_t * hashtable);

// add a new string to the hashtable
void hashtable_add(hashtable_t * hashtable, const char *s);

// remove a string from the hashtable; if the string
// doesn't exist in the hashtable, do nothing
void hashtable_remove(hashtable_t * hashtable, const char *s);

// print the contents of the hashtable
void hashtable_print(hashtable_t * hashtable);



  
void list_insert_ordered(char *name,  struct node **head);

void list_insert(char *name, struct node **head);

void list_remove(char *name, struct node **head, pthread_mutex_t * linklock);

void list_dump(struct node *list);
    
void list_clear(struct node *list);

#endif

