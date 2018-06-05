/**
 * Implements a dictionary's functionality.
 */

#include <stdbool.h>

#include "dictionary.h"

/**
 * Returns true if word is in dictionary else false.
 */
 
Trie* root;
int wordSize = 0;
 
bool check(const char *word)
{
    //return false if word length is 0
    if(strlen(word) == 0)
        return false;
    
    //char storage, char index and counter variable
    int c, num, count = 0;
    
    //trav node
    Trie* trav = root;

    while((c = word[count]) != '\0'){
        //calculate character's position in the array
        if(isalpha(c))
            num = tolower(c) - 97;
        else if(c == '\'')
            num = 26;
        
        //follow the node if char exists, else return false
        if(trav->children[num] != NULL)
            trav = trav->children[num];
        else
            return false;
        
        count++;
    }
    
    //return isWord after the word ends
    return trav->isWord;
    
}

/**
 * Loads dictionary into memory. Returns true if successful else false.
 */
bool load(const char *dictionary)
{
    //open the dictionary
    FILE *dict = fopen(dictionary, "r");
    
    //exit if dict is NULL
    if(dict == NULL){
        fprintf(stderr, "Couldn't open dictionary.");
        return false;
    }
    
    //store character and its index
    int c, num;
    
    //root and current node
    Trie *curTrie;
    curTrie = root = create();
    
    //Loop to retrieve the characters
    do{
        //get the character and create its index
        c = fgetc(dict);
        
        //set its index value if c is a valid character 
        if(isalpha(c))
            num = c - 97;
        else if(c =='\'')
            num = 26;
        
        //if its a valid char then follow/create the node
        if(isalpha(c) || c == '\''){
            
            //create node if isn't there
            if(curTrie->children[num] == NULL)
                curTrie->children[num] = create();
                
            //follow the node
            curTrie = curTrie->children[num];
        }
        
        //mark the end of the word and set current pointer to root
        //to prepare for the next word
        else{
            curTrie->isWord = true;
            wordSize++;
            curTrie = root;
        }
    }while(c != EOF);//continue loop until EOF is reached
    
    //set root's isWord to false because of the loop's addition
    wordSize--;
    root->isWord = false;
    
    //close file
    fclose(dict);
    
    return true;
}

/**
 * Returns number of words in dictionary if loaded else 0 if not yet loaded.
 */
unsigned int size(void)
{
    return wordSize;
}

/**
 * Unloads dictionary from memory. Returns true if successful else false.
 */
 
bool unloadNode(Trie* node){
    
    //trav node
    Trie* trav = node;
    
    //loop to check every child
    for(int i = 0; i < NUM_NODE; i++){
        if(trav->children[i] != NULL && !unloadNode(trav->children[i]))
            return false;
    }
    
    //if loop is done, then every element is NULL
    //therefore free is applicable
    free(trav);
    
    //return true after freeing
    return true;
    
}
bool unload(void)
{
    return unloadNode(root);
}

/**
 * Creates a node with dynamically allocated meomory. Returns the pointer of the node if successful
 */
Trie* create(void){
    
    //Create Trie with malloc
    Trie* newTrie = malloc(sizeof(Trie));
    
    //Exit if memory wasn't granted
    if(newTrie == NULL){
        fprintf(stderr, "Couldn't get enough memory.");
        exit(1);
    }
    
    //Initialize it with value and NULL pointers
    newTrie->isWord = false;
    for(int i = 0; i < NUM_NODE; i++)
        newTrie->children[i] = NULL;
    
    //return the new Trie's pointer
    return newTrie;
    
}
