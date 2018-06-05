#include <stdio.h>
#include <cs50.h>
#include <ctype.h>
#include <string.h>

int main(void){
    
    string name = get_string();
    bool flag = false;
    for(int i = 0, n = strlen(name); i < n; i++){
        if(flag == false && name[i] != ' '){
            printf("%c", toupper(name[i]));
            flag = true;
        }
        if(name[i] == ' ')
            flag = false;
    }
    printf("\n");
}