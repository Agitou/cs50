#include <cs50.h>
#include <stdio.h>

int minToBot(int min);

int main(void){

	printf("Minutes: ");
	int min = get_int();

	printf("Bottles: %d\n", minToBot(min));
}

int minToBot(int min){
	return min * 12;
}