/**
 * helpers.c
 *
 * Helper functions for Problem Set 3.
 */
 
#include <cs50.h>
#include <stdio.h>

#include "helpers.h"

/**
 * Returns true if value is in array of n values, else false.
 */
bool search(int value, int values[], int n)
{
    int low = 0;
    int high = n - 1;
    int mid = (low + high) / 2;
    
    while(high >= low){
        if(values[mid] == value)
            return true;
        else if(values[mid] < value)
            low = mid + 1;
        else
            high = mid - 1;
        mid = (low + high) / 2;
    }
    return false;
}

/**
 * Sorts array of n values.
 */
void sort(int values[], int n)
{
    
    for(int i = 1; i < n; i++){
        
        int j = i;
        
        while(j >= 0 && values[j] < values[j - 1]){
            int temp = values[j];
            values[j] = values[j - 1];
            values[j - 1] = temp;
            j--;
        }
    
    }
    
    return;
}

