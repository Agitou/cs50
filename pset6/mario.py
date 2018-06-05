#Loop to get a positive integer from the user
print("Height: ", end="")
while True:
    height = int(input())
    if height > 0:
        break
    else:
        print("Retry")

#for loop to print out the mario blocks
for i in range(height):
    
    #print the extra spaces at the start
    for j in range(height - i):
        print(" ", end="")
        
    #print the blocks on the left side
    for j in range(2*(i + 1) + 1):
        
        #print the middle line if j is equal to row number i + 1
        if j == i + 1:
            print(end=" ")
        
        #else just print out the blocks
        else:
            print("#", end="")
    
    #print out a new line    
    print()

        
        
