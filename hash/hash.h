#ifndef __HASH_H__

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>

typedef struct {
	// Array used to map alphabetical characters to 
	int* charval;
	char** table;
	int numletters;
	int tablesize;
	
} hashtable_t;

//Used to resize and rehash.
hashtable_t *resize(hashtable_t* table);

//Used to get hash value (the location in our char* table)
int hashFunction(const char* string, int numletters, int* charval);

// create a new hashtable; parameter is a size hint
hashtable_t *hashtable_new(int sizehint);

// free anything allocated by the hashtable library
void hashtable_free(hashtable_t * slave);

// add a new string to the hashtable
void hashtable_add(hashtable_t *, const char *);

// remove a string from the hashtable; if the string
// doesn't exist in the hashtable, do nothing
void hashtable_remove(hashtable_t *, const char *);

// print the contents of the hashtable
void hashtable_print(hashtable_t *);

#endif

