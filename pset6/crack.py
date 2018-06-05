import sys
import crypt
import string
import time
from hmac import compare_digest as compare_hash

#character array and its size in a variable
chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
charsNum = len(chars)

def main():

    #check system arguments
    if(len(sys.argv) != 2):
        print("Usage: python crack.py hash")
        exit(1)
    
    #store hash
    inputHash = sys.argv[1]
    
    #max number of length
    maxLength = 4
    
    #loop integer
    i = 1
    
    begin = time.process_time()
    #check every word from length 1 - maxLength
    for i in range(maxLength + 1):
        wordGenerator(i, inputHash)
        
    end = time.process_time()
    
    print("elapsed time {}".format(end - begin))
    
#generates all words of a given length
def wordGenerator(wordLength, hashWord):
    
    #declare chars and charsNum as global variable
    global chars, charsNum
    
    #loop integer, entry array
    entry = [0] * wordLength
    
    #word array
    word = [None] * wordLength
    
    #loop to generate the words
    while True:
        
        #sets word elements to the alphabet of position entry[i]
        for i in range(wordLength):
            word[i] = chars[int(entry[i])]
        
        #check the password
        if checkPass(''.join(word), hashWord):
            print("Correct password: {}".format(''.join(word)))
            return
            
        
        j = 0
        #update entry elements if needed
        while(j < wordLength):
            #increment entry
            entry[j] += 1
            
            #if entry[i] hasn't reached 'z' then exit the loop
            if charsNum is not entry[j]:
                break
            
            #else set it to 0
            entry[j] = 0
            
            #increment loop variable
            j += 1
            
        #breaks out of loop when all words are generated
        if(j >= wordLength):
            break
    
#check if pass is true    
def checkPass(generatedPass, userHash):
    
    #generate hash from the given password
    generatedHash = crypt.crypt(generatedPass, "50")
    
    #return the boolean value
    return compare_hash(generatedHash, userHash)

    
if __name__ == "__main__":
    main()