#include <stdio.h>
#include <cs50.h>
#include <string.h>
#include <ctype.h>

int main(int argc, string argv[]){
    
    if(argc == 1 || argc > 2){
        printf("Usage: ./vigenere k\n");
        return 1;
    }
    
    else if(argc == 2){
        for(int i = 0, len = strlen(argv[1]); i < len; i++)
        if(!isalpha(argv[1][i])){
            printf("Keyword must be only A-Z and a-z");
            return 1;
        }
    }
    string key = argv[1];
    
    printf("plaintext: ");
    string pt = get_string();
    
    for(int i = 0, j = 0, len = strlen(pt); i < len; i++, j++){
        int minusCons = 65;
        int keyMinus = 65;
        
        if(!isalpha(pt[i])){
            j--;
            continue;
        }
        
        if(islower(pt[i]))
            minusCons += 32;
            
        if(j == strlen(key))
            j = 0;
            
        if(islower(key[j]))
            keyMinus += 32;
        
        int c = (pt[i] - minusCons + (key[j] - keyMinus) % 26 ) % 26;
        
        pt[i] = (char) (c + minusCons);
    }
    
    printf("ciphertext: %s\n", pt);
    return 0;
    
    
}   