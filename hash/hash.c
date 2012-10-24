#include "hash.h"


// create a new hashtable; parameter is a size hint
hashtable_t *hashtable_new(int sizehint) {
	hashtable_t * hash = malloc(sizeof(hashtable_t));	
	hash->charval = malloc(sizeof(int)*26);
	int i = 0;
	for(; i<26; i++){
		(hash->charval)[i] = i+1;
	}
	if(sizehint<0){
		sizehint*=-1;
	}
	hash-> numletters = (sizehint/(26*26)) + 1; //This will give us a minimum array size of 676.
	hash -> table = malloc(sizeof(char*) * (26*26)*(hash->numletters));
    return hash;
}

//Used to resize and rehash.
hashtable_t *resize(hashtable_t* hashtable){
	int tablelen = (hashtable->numletters) * (26*26);
	hashtable_t* bigger = hashtable_new(tablelen);
	int i = 0;
	for(; i < tablelen; i++){
		if((hashtable->table)[i] != NULL ){
			hashtable_add(bigger, (hashtable->table)[i]);
		}
	}
	hashtable_free(hashtable);
	return bigger;
}

//Used to get hash value (the location in our char* table)
int hashFunction(const char* string, int numletters, int* charval){
	int hashval = 0;
	int letterval = 0;
	int i = 0; //Currently we're just going to hash from the front of a string, but it might be more unique to hash from (roughly) the middle...
	for(; i < numletters && i < strlen(string); i++){
		if(string[i] >= 'A' && string[i] <='Z'){
			letterval = string[i] - 'A';
		} else if(string[i] >= 'a' && string[i] <='z'){
			letterval = string[i] - 'a';
		}else{
			letterval = -1;
		}
		if(letterval>-1){
			hashval+= charval[letterval] * charval[letterval];
		}
	}
	return hashval;
}

// free anything allocated by the hashtable library
void hashtable_free(hashtable_t *hashtable) {
	int tablelen = (hashtable->numletters) * (26*26);
	int i = 0;
	for(; i<tablelen; i++){
		free((hashtable->table)[i]);
	}
	free(hashtable->table);
	free(hashtable->charval);
	free(hashtable);
	return;
}

// add a new string to the hashtable
void hashtable_add(hashtable_t *hashtable, const char *s) {
	int tablelen = (hashtable->numletters) * (26*26);
	int hashVal = hashFunction(s, hashtable-> numletters, hashtable-> charval);
	if( hashVal >= tablelen){
		hashtable = resize(hashtable);
		hashVal = hashFunction(s, hashtable-> numletters, hashtable-> charval);
	}
	if((hashtable->table)[hashVal] == NULL){
		(hashtable->table)[hashVal] = strdup(s); //THIS MALLOCS SOME DAMN MEMORY!!
	} else if(strcmp(s, (hashtable->table)[hashVal]) == 0){
		return;
	} else{
		int i = 1;
		int go = 1;
		int firstspace = -1;
		if((hashtable->table)[hashVal][0] == '~'){
			firstspace = hashVal;
		}
		while(go == 1){
			if((hashVal+i)%tablelen == hashVal){
				hashtable = resize(hashtable);
				hashVal = hashFunction(s, hashtable-> numletters, hashtable-> charval);
				go = 0;
			} else if((hashtable->table)[(hashVal+i)%tablelen][0] == '~'){
				if(firstspace == -1){
					firstspace = (hashVal+i)%tablelen;
				}
			} else if(strcmp(s, (hashtable->table)[(hashVal+i)%tablelen]) == 0){
				return;
			} else if((hashtable->table)[(hashVal+i)%tablelen] == NULL){
				if(firstspace==-1){
					firstspace = (hashVal+i)%tablelen;
					go = 0;
				} else{
					go = 0;
				}
			} 
		}
		free((hashtable->table)[firstspace]);
		(hashtable->table)[firstspace] = strdup(s);
	}

	return;
}

//We are going to have '~' represent a place that had an item removed.

// remove a string from the hashtable; if the string
// doesn't exist in the hashtable, do nothing
void hashtable_remove(hashtable_t *hashtable, const char *s) {
	int tablelen = (hashtable->numletters) * (26*26);
	int hashVal = hashFunction(s, hashtable-> numletters, hashtable-> charval);
	if(strcmp(s, (hashtable->table)[hashVal]) == 0){
		(hashtable->table)[hashVal][0] = '~';
	} else if((hashtable->table)[hashVal] == NULL){
		return;
	} else if((hashtable->table)[hashVal][0] == '~'){
		int i = 1;
		for(; (hashVal+i)%tablelen != hashVal; i++){
			if((hashtable->table)[(hashVal+i)%tablelen] == NULL){
				return;
			}else if(strcmp((hashtable->table)[(hashVal+i)%tablelen], s) == 0){
				(hashtable->table)[(hashVal+i)%tablelen][0] = '~';
				return;
			}
		}
	}
	return;
}

// print the contents of the hashtable
void hashtable_print(hashtable_t *hashtable) {
	int tablelen = (hashtable->numletters) * (26*26);
	int i = 0;
	for(; i<tablelen; i++){
		if((hashtable->table)[i] != NULL && (hashtable->table)[i][0] != '~'){
			printf("%s\n", (hashtable->table)[i]);
		}
	}

	return;
}


