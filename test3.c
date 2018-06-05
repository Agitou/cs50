#include <stdio.h>

typedef struct trie{
    bool isWord;
    struct trie* branch[NUM_NODE];
}Trie;

int main(void){
    int c = '\'';
    
    char array[45];
    
    array[c] = 'g';
    
    printf("character: %c\n", array[39]);
}