#include <stdio.h>
#include <cs50.h>

int main(void){
    
    printf("Height: ");
    int height = get_int();
    
    for(int i = 0; i < height; i++){
        
        for(int j = 0; j < height - i; j++){
            printf(" ");
        }
        
        for(int j = 0; j <= i; j++){
            printf("#");
        }
        
        printf("  ");
        
        for(int j = i; j >= 0; j--){
            printf("#");
        }
        
        printf("\n");
    }
}