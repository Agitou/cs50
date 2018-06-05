#include <stdio.h>
#include <cs50.h>
#include <string.h>
#include <ctype.h>

int main(int argc, string argv[]){
    
    if(argc == 1){
        printf("Usage: ./casesar k\n");
        return 1;
    }
    
    //Get key    
    int key = atoi(argv[1]);
    //char letters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    
    //Ask and get plaintext
    printf("plaintext: ");
    string pt = get_string();
    
    for(int i = 0, len = strlen(pt); i < len; i++){
        int minusCons = 65;
        
        if(!isalpha(pt[i]))
            continue;
        
        if(islower(pt[i]))
            minusCons += 32;
            
        int c = (pt[i] - minusCons + key) % 26;
        printf("char: %d\n", c);
        pt[i] = (char) (c + minusCons);
    }
    
    printf("ciphertext: %s\n", pt);
    return 0;
    
    
}   