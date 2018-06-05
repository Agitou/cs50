#include <stdio.h>
#include <cs50.h>
#include <ctype.h>
#include <string.h>

int main(void){
    
    string name = get_string();
    for(int i = 0, n = strlen(name); i < n; i++){
        if(i == 0 && name[i] != ' ')
            printf("%c", toupper(name[i]));
        else if(name[i - 1] == ' ' && name[i] != ' ')
            printf("%c", toupper(name[i]));
    }
    printf("\n");
}