#include "hash.h"

/*
NICKIE VANMETER AND SEBASTIAN SANGERVASI
THEY MADE A THREADSAFE HASHTABLE. THESE ARE WORDS.
IT'S NICE BECAUSE THREADS CAN ADD TO SEPARATE LINKED LISTS CONCURRENTLY.
WE LIKE THIS.
*/

// create a new hashtable; parameter is a size hint
hashtable_t *hashtable_new(int sizehint) {
	hashtable_t * hash = malloc(sizeof(hashtable_t));
	int primes[68] = {11,31,73,127,179,233,283,353,419,467,547,607,661,739,811,877,947,1019,1087,1153,1229,1297,1381,1453,1523,1597,1663,1741,1823,1901,1993,2063,2131,2221,2293,2371,2437,2539,2621,2689,2749,2833,2909,3001,3083,3187,3259,3343,3433,3517,3581,3659,3733,3823,3911,4001,4073,4153,4241,4327,4421,4507,4591,4663,4759,4861,4943,5009};
	int i = 0;
	if(sizehint>primes[67]){
		hash->tablesize = sizehint + (sizehint%2);
	}else{	
		for(; i<68; i++){
			if(primes[i]> sizehint){
				hash->tablesize = primes[i];
				break;
			}
		}
	}
	hash -> table = malloc(sizeof(struct node*) * (hash->tablesize) );
	hash->listlocks = malloc(sizeof(pthread_mutex_t)*(hash->tablesize));
	pthread_mutex_init(&(hash->hashlock), NULL);
    return hash;
}

//Used to get hash value (the location in our char* table)
int hashFunction(const char* string, int tablelen){
	int hashval = 0;
	int letterval = 0;
	int slen = strlen(string);
	int count = 0;
	for(; count < slen; count++){
		if(string[count] >= 'A' && string[count] <='Z'){
			letterval = string[count] - 'A';
		} else if(string[count] >= 'a' && string[count] <='z'){
			letterval = string[count] - 'a'; 
			//printf("oh good\n");
		}else{
			letterval = -1;
		}
		if(letterval>-1){
			hashval+= (letterval+1)*(letterval+1);
		}
	}
	hashval = hashval%tablelen;
	return hashval;
}

// free anything allocated by the hashtable library
void hashtable_free(hashtable_t *hashtable) {
	int tablelen = hashtable->tablesize;
	int i = 0;
	pthread_mutex_lock(&(hashtable->hashlock));
	for(; i<tablelen; i++){
		list_clear((hashtable->table)[i]);
		pthread_mutex_destroy(&(hashtable->listlocks)[i]);
	}
	free(hashtable->table);
	pthread_mutex_unlock(&(hashtable->hashlock));
	pthread_mutex_destroy(&(hashtable->hashlock));
	free(hashtable->listlocks);
	free(hashtable);
	
	return; 
}

// add a new string to the hashtable
void hashtable_add(hashtable_t *hashtable, const char *s) {
	//printf("Added: |%s|\n",s );
	int tablelen = hashtable->tablesize;
	int hashVal = hashFunction(s, tablelen);
	pthread_mutex_lock(&(hashtable->hashlock));
	if((hashtable->table)[hashVal] == NULL){
		(hashtable->table)[hashVal] = malloc(sizeof(struct node)); //THIS MALLOCS SOME DAMN MEMORY!!
		list_insert((char*)s, &(hashtable->table)[hashVal]);
		//(hashtable->listlocks)[hashVal] = malloc(sizeof(pthread_mutex_t));
		pthread_mutex_init(&(hashtable->listlocks)[hashVal], NULL);
		pthread_mutex_unlock(&(hashtable->hashlock));	
	} else{
		pthread_mutex_lock(&(hashtable->listlocks)[hashVal]);
		pthread_mutex_unlock(&(hashtable->hashlock));
		list_insert_ordered((char*)s, &(hashtable->table)[hashVal]);
		pthread_mutex_unlock(&(hashtable->listlocks)[hashVal]);		
	}

	return;
}


// remove a string from the hashtable; if the string
// doesn't exist in the hashtable, do nothing
void hashtable_remove(hashtable_t *hashtable, const char *s) {
	int tablelen = hashtable->tablesize;
	int hashVal = hashFunction(s, tablelen);
	pthread_mutex_lock(&(hashtable->hashlock));//big
	if((hashtable->table)[hashVal] != NULL){
		pthread_mutex_lock(&(hashtable->listlocks)[hashVal]);
		pthread_mutex_unlock(&(hashtable->hashlock));//big
		list_remove((char*)s, &(hashtable->table)[hashVal], &(hashtable->listlocks)[hashVal]);
	} else{
		pthread_mutex_unlock(&(hashtable->hashlock));//big
	}
		
	return;
}

// print the contents of the hashtable
void hashtable_print(hashtable_t *hashtable) {
	int tablelen = hashtable->tablesize;
	int i = 0;
	pthread_mutex_lock(&(hashtable->hashlock));//big
	for(; i<tablelen; i++){
		if((hashtable->table)[i] != NULL){
			printf("I haz a bucket\n");
			list_dump((hashtable->table)[i]);
		}
	}
	pthread_mutex_unlock(&(hashtable->hashlock));//big
	
	return;
}

     
void list_insert_ordered(char *name,  struct node **head) 
{
	struct node *newnode = malloc(sizeof(struct node));
	strncpy( (*newnode).name, name, 127);
	if(NULL == *head){
		*head = newnode;
		return;
	}
	struct node *pre = NULL;
	struct node *post = *head;
	while(NULL != post && strcasecmp(post->name, name) < 1){
		pre = post;
		post = (*post).next;
	}
	(*newnode).next = post;
	if(NULL == pre){
		*head = newnode;
		return;	
	}
	(*pre).next = newnode;
	return;  
}
    
    
void list_insert(char *name, struct node **head) {
     struct node *newnode = malloc(sizeof(struct node));
     strncpy(newnode->name, name, 127);
     newnode->next = *head;
     *head = newnode;
	 (*head)->next = NULL;
}

void list_dump(struct node *list) {
    while (list != NULL) {
		if(list->name != NULL){
        	printf("%s\n", list->name);
        }
		list = list->next;
    }
}
    
void list_clear(struct node *list) {
    while (list != NULL) {	
        struct node *tmp = list;
        list = list->next;
        free(tmp);
    }
}

void list_remove(char *name, struct node **head, pthread_mutex_t *linklock) {
    struct node *drop = *head;
	struct node *prev = *head;
	if(strcmp(drop->name, name) == 0){
		*head = (*head)->next;
		free(drop);
		pthread_mutex_unlock(linklock);
		pthread_mutex_destroy(linklock);
		return;
	}
	while((drop)!= NULL && strcmp(drop->name, name) != 0){
		prev = drop;		
		drop = drop->next;

	}
    if(drop!=NULL){
		prev->next = drop->next;
		free(drop);
	}
	pthread_mutex_unlock(linklock);
	return;
}


