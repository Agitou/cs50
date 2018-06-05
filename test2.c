/*
 * C program to create a linked list and display the elements in the list
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/time.h>
#include <sys/resource.h>
 
#define NUM_NODE (27)
#define LENGTH 45

#undef gerusage
#undef calculate

typedef struct trie{
    bool isWord;
    struct trie *children[NUM_NODE];
}Trie;

Trie* root;

Trie* create(void);
unsigned int size();
bool load(const char *dictionary);
bool unload();
bool check(const char* word);
double calculate(const struct rusage *b, const struct rusage *a);


int main()
{
    //open the dictionary
    char* dictionary = "large";

    //struct rusages for time calculation
    struct rusage before, after;
    
    //durations
    double timeLoad = 0.0, timeSize = 0.0, timeUnload = 0.0, timeCheck = 0.0;
    
    getrusage(RUSAGE_SELF, &before);
    bool loaded = load(dictionary);
    // abort if dictionary not loaded
    if (!loaded)
    {
        printf("Could not load %s.\n", dictionary);
        return 1;
    }
    getrusage(RUSAGE_SELF, &after);
    
    //calculate timeLoad
    timeLoad = calculate(&before, &after);
    
    // try to open text
    char *text = "alice.txt";
    FILE *fp = fopen(text, "r");
    if (fp == NULL)
    {
        printf("Could not open %s.\n", text);
        unload();
        return 1;
    }
    
     // prepare to report misspellings
    printf("\nMISSPELLED WORDS\n\n");

    // prepare to spell-check
    int index = 0, misspellings = 0, words = 0;
    char word[LENGTH+1];

    // spell-check each word in text
    for (int c = fgetc(fp); c != EOF; c = fgetc(fp))
    {
        // allow only alphabetical characters and apostrophes
        if (isalpha(c) || (c == '\'' && index > 0))
        {
            // append character to word
            word[index] = c;
            index++;

            // ignore alphabetical strings too long to be words
            if (index > LENGTH)
            {
                // consume remainder of alphabetical string
                while ((c = fgetc(fp)) != EOF && isalpha(c));

                // prepare for new word
                index = 0;
            }
        }

        // ignore words with numbers (like MS Word can)
        else if (isdigit(c))
        {
            // consume remainder of alphanumeric string
            while ((c = fgetc(fp)) != EOF && isalnum(c));

            // prepare for new word
            index = 0;
        }

        // we must have found a whole word
        else if (index > 0)
        {
            // terminate current word
            word[index] = '\0';

            // update counter
            words++;

            // check word's spelling
            getrusage(RUSAGE_SELF, &before);
            bool misspelled = !check(word);
            getrusage(RUSAGE_SELF, &after);

            // update benchmark
            timeCheck += calculate(&before, &after);

            // print word if misspelled
            if (misspelled)
            {
                printf("%s\n", word);
                misspellings++;
            }

            // prepare for next word
            index = 0;
        }
    }

    // check whether there was an error
    if (ferror(fp))
    {
        fclose(fp);
        printf("Error reading %s.\n", text);
        unload();
        return 1;
    }

    // close text
    fclose(fp);
    
    //calculate size function's time
    getrusage(RUSAGE_SELF, &before);
    unsigned int numWords = size();
    getrusage(RUSAGE_SELF, &after);
    
    //assign the time to a variable
    timeSize = calculate(&before, &after);
    
    //unload dictionary
    getrusage(RUSAGE_SELF, &before);
    bool unloaded = unload();
    getrusage(RUSAGE_SELF, &after);
    
    //calculate the time it took to unload
    timeUnload = calculate(&before, &after);
    
    (unloaded) ? printf("Unloaded successfully\n") : printf("Failed to unload\n");
    
    //print out the benchmarks
    printf("size: %i\n", numWords);
    printf("timeSize: %.2f\n", timeSize);
    printf("timeLoad: %.2f\n", timeLoad);
    printf("timeUnload: %.2f\n", timeUnload);
    printf("timeCheck: %.2f\n", timeCheck);
    
}

bool load(const char *dictionary){
    //open the dictionary
    FILE *dict = fopen(dictionary, "r");
    
    //exit if dict is NULL
    if(dict == NULL){
        fprintf(stderr, "Couldn't open small.txt.");
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
            if(curTrie != root)
                curTrie->isWord = true;
            curTrie = root;
        }
    }while(c != EOF);//continue loop until EOF is reached
    
    //close file
    fclose(dict);
    
    return true;
}

unsigned int sizeNode(Trie* node){
    //variable to count
    int count = 0;
    
    //trav node
    Trie* trav = node;
    
    //recursion to go through every node
    for(int k = 0; k < NUM_NODE; k++){
            
        //check if children is NULL or not
        if(trav->children[k] != NULL){
            
            //then check if its the end of a word
            //increment if it is
            if(trav->children[k]->isWord)
                count++;
            
            //check the children's child elememts
            count += sizeNode(trav->children[k]);
        }
        
    }
    
    return count;
}
unsigned int size(void)
{
    return sizeNode(root);
}
bool unloadNode(Trie* node){
    
    //trav node
    Trie* trav = node;
    
    //loop to check every child
    for(int i = 0; i < NUM_NODE; i++){
        if(trav->children[i] != NULL && unloadNode(trav->children[i]) == false)
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

double calculate(const struct rusage *b, const struct rusage *a){
    if (b == NULL || a == NULL)
    {
        return 0.0;
    }
    else
    {
        return ((((a->ru_utime.tv_sec * 1000000 + a->ru_utime.tv_usec) -
                 (b->ru_utime.tv_sec * 1000000 + b->ru_utime.tv_usec)) +
                ((a->ru_stime.tv_sec * 1000000 + a->ru_stime.tv_usec) -
                 (b->ru_stime.tv_sec * 1000000 + b->ru_stime.tv_usec)))
                / 1000000.0);
    }
}



