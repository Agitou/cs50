#NO1111111111111111
#get the value of q and add "%" for querying
    args = [arg.strip() for arg in request.args.get("q").split(',')]
    
    print(args)
    
    #db command to execute
    cargs = {'place_name': '%', 'admin_name1': '%', 'postal_code': '%'}
    
    #add conditions to the command with a maximum of 3
    for i in range(len(args)): 
            
        #check for the type of args[i]
        try:
            if cargs['postal_code'] is None:
                cargs['postal_code'] = str(int(args[i])) + "%"
            
        #add the commands as place_name or admin_name1 if it isnt postal_code    
        except ValueError:
            
            #if place_name is None fill it
            if cargs['place_name'] is None:
                cargs['place_name'] = args[i] + "%"
            
            #else fill admin_name1
            elif cargs['admin_name1'] is None:
                cargs['admin_name1'] = args[i] + "%"
                
    print(cargs)
    
    places = db.execute("SELECT * FROM places WHERE place_name LIKE :pn AND admin_name1 LIKE :an1 AND postal_code LIKE :pc", 
                pn=cargs['place_name'], an1=cargs['admin_name1'], pc=cargs['postal_code'])
    
    return jsonify(places)
    
    
#NO22222222222
"""Search for places that match query."""

    #get the value of q and add "%" for querying
    
    q = request.args.get("q") + "%"
 
    #check if q is postal_code or the other two: place_name, admin_name1
    
    #check for postal_codes in the db when q is int
    try:
        int(q[:-1])
        places = db.execute("SELECT * FROM places WHERE postal_code LIKE :q", q=q[:-1])
    
    #else check for place_name and admin_name1 
    except ValueError:
        places = db.execute("SELECT * FROM places WHERE place_name LIKE :q OR admin_name1 LIKE :q", q=q[:-1])
    
    #output the data of the places
    return jsonify(places)