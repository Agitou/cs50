#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef uint8_t BYTE;

typedef enum { false, true } bool;

int main(int argc, char *argv[]){
    
    //ensure proper usage
    if(argc != 2){
        fprintf(stderr, "Usage: ./recover image");
        return(1);
    }
    
    //remember card file name, create EOF checker, filename's integer
    char *memCardName = argv[1];
    int count = 0;
    
    //Create buffer and comparison array
    BYTE buffer[512];
    BYTE compare[4] = {0xff, 0xd8, 0xff, 0xe0};
    
    //open memory card for read
    FILE *memCard = fopen(memCardName, "r");
    if (memCard == NULL){
        fprintf(stderr, "Could not open %s.\n", memCardName);
        return 2;
    }

    
    //file for images
    char *filename = malloc(7);
    if(filename == NULL){
        fprintf(stderr, "Couldn't get enough memory for %s.\n", filename);
        return 3;
    }
    FILE *img;
    
    //boolean for alreadyFound
    bool alreadyFound = false;
    
    //checking mem card until the end
    while(fread(buffer, 512, 1, memCard)){
        
        //check if it's a new start
        if(buffer[0] == compare[0] && buffer[1] == compare[1] && 
            buffer[2] == compare[2] && (buffer[3] & 0xf0) == compare[3]){
                
                //if there isn't a JPEG already found, create new file
                if(alreadyFound)
                    fclose(img);
                
                //Create new file and write in it
                sprintf(filename, "%03i.jpg", count);
                img = fopen(filename, "w");
                if(img == NULL){
                    fprintf(stderr, "Could not open %s.\n", filename);
                    return 3;
                }
                fwrite(buffer, 512, 1, img);
                    
                //set alreadyFound to true
                alreadyFound = true;
                    
                //increment filename's integer
                count++;
        }
        
        //if it's not a new start
        else
            //if there is already a JPEG found, write it to the current file
            if(alreadyFound)
                fwrite(buffer, 512, 1, img);
            
        
    }
    
    fclose(memCard);
    return(0);
    
}