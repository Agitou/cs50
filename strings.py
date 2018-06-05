def main():
    
    x = 5
    y = 6
    
    print("x = {}".format(x))
    print("y = {}".format(y))
    x, y = y, x
    print()
    print("x = {}".format(x))
    print("y = {}".format(y))
    
if __name__ == "__main__":
    main()