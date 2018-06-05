#include <stdio.h>
#include <cs50.h>
#include <math.h>

bool isValid(long cardNum, int numOfDigits);
void compIdent(long cardNum, int numOfDigits);

int main(void){
    
    printf("Number: ");
    long ccNum;
    while((ccNum = get_long_long()) <= 0){
        printf("Retry: ");
    }
    
    int numOfDigits = floor(log10(ccNum)) + 1;
    if(isValid(ccNum, numOfDigits))
        compIdent(ccNum, numOfDigits);
    else
        printf("INVALID\n");
        
    printf("%i\n", 0 % 2);
}

bool isValid(long cardNum, int numOfDigits){
    
    int prodSum = 0;
    int normSum = 0;
    bool isValid = false;
    
    for(int i = 0; i < numOfDigits; i++){
        int curDigit = cardNum % 10;
        
        if(i % 2 == 0){
            normSum += curDigit;
        }
        else{
            printf("Odd: %i\n", curDigit);
            prodSum += (2 * curDigit) % 10 + (2 * curDigit) / 10;
        }
        
        cardNum /= 10;
    }
    
    
    if((normSum + prodSum) % 10 == 0){
        isValid = true;
    }

    return isValid;
}

void compIdent(long cardNum, int numOfDigits){
    
    int iniTwoDig = cardNum / pow(10, (numOfDigits - 2));
    
    if(numOfDigits == 15 && (iniTwoDig == 34 || iniTwoDig == 37))
        printf("AMEX\n");
    else if(numOfDigits == 16 && (iniTwoDig >= 51 && iniTwoDig <= 55))
        printf("MASTERCARD\n");
    else if((numOfDigits == 13 || numOfDigits == 16) && iniTwoDig / 10 == 4)
        printf("VISA\n");
    else
        printf("Unknown\n");
}

