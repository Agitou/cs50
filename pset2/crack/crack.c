#define _XOPEN_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

void wordGenerator(int wordLength, const char* inputHash);
bool checkPass(const char* key, const char* hash);


int len = 4;
int count = 0;
char letters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
int nbletters = sizeof(letters)-1;

int main(int argc, char* argv[]) {
    
    //exits when command line arguments aren't correct
    if(argc != 2){
        fprintf(stderr, "Usage: ./crack hash\n");
        return 1;
    }
    
    //stores given hash    
    char* userHash = argv[1];
 
    //tries out all words of length from 1 - len
    clock_t begin = clock();
    for(int i = 1; i <= len; i++){
        wordGenerator(i, userHash);
    }
    clock_t end = clock();
    
    double timeSpent = (double) (end - begin) / CLOCKS_PER_SEC;
    
    printf("Time spent:  %f\n", timeSpent);
}

//generates all the words of a given length
void wordGenerator(int wordLength, const char* inputHash){
    
    //loop integer, array to store letter pos
    int i, entry[wordLength];
    
    //array to store word
    char pass[wordLength + 1];
    
    //initialiazes entry array
    for(i = 0 ; i < wordLength ; i++) 
    	entry[i] = 0;
    
    //loop to generate words 	
    do {
        //sets pass[i] to the alphabet of position entry[i]
        //for the given length
        for(i = 0 ; i < wordLength ; i++) 
        	pass[i] = letters[entry[i]];
        	
        //stops string from being combined with elements in memory
        pass[wordLength] = '\0';
        
        printf("%s\n", pass);
        
        //exits when password is given 
        if(checkPass(pass, inputHash)){
            fprintf(stdout, "Correct password: %s\n", pass);
            return;
        }
        
        //sets entry elemnts to 0 when they've reached the limit
        for(i = 0 ; i < wordLength && ++entry[i] == nbletters; i++)
        	entry[i] = 0;
        
    } while(i < wordLength);
    
    
}

bool checkPass(const char* password, const char* hash){
    if(strcmp(crypt(password, "50"), hash) == 0){
            return true;
    }
    return false;
}