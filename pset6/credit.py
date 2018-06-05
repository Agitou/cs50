import math
#main function
def main():
    #Prompt for card number
    print("Number: ", end="")
    
    #Loop to check integer
    while True:
        #Parse string to int
        cardNum = int(input())
        
        #Break if cardNum is positive
        if cardNum > 0:
            break
        else:
            print("Retry: ", end="")
            
    #Check the validity of the card number
    if isValid(cardNum):
        #Get card provider depending on the initial 2 integer
        print("{}".format(cardProv(cardNum)))
    else:
        print("INVALID")
    
#card number checking function    
def isValid(ccNum):
    #get number of digits
    numOfDigits = len(str(ccNum))

    oddSum = evenSum = 0
    
    #return false if card length isn't correct
    if numOfDigits < 13 or numOfDigits > 16:
        return False
        
    #loop to sum the digits individually 
    for i in range(numOfDigits):
        #get current digit
        curDigit = ccNum % 10
        
        #for digits in even position
        if(i % 2 == 0):
            oddSum += curDigit
            
        #for digits in odd position
        else:
            curDigit = 2 * curDigit;
            evenSum += curDigit % 10 + curDigit // 10
            
        ccNum = ccNum // 10;
        
    return (evenSum + oddSum) % 10 == 0
    
def cardProv(ccNum):
    #len of ccNum
    cardLen = len(str(ccNum))

    #first 2 digit
    initTwo = ccNum // math.pow(10, (cardLen - 2))

    #AMEX
    if cardLen == 15 and (initTwo == 34 or initTwo == 37):
        return "AMEX"
    #MASTER
    elif cardLen == 16 and (initTwo >= 51 and initTwo <= 55):
        return "MASTERCARD"
    
    return "VISA"
        
    
if __name__ == "__main__":
    main()
    
#378282246310005
#5105105105105100