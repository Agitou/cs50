import os
import re
from flask import Flask, jsonify, render_template, request, url_for
from flask_jsglue import JSGlue

from cs50 import SQL
from helpers import lookup

# configure application
app = Flask(__name__)
JSGlue(app)

# configure CS50 Library to use SQLite database
db = SQL("sqlite:///mashup.db")

# ensure responses aren't cached
if app.config["DEBUG"]:
    @app.after_request
    def after_request(response):
        response.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
        response.headers["Expires"] = 0
        response.headers["Pragma"] = "no-cache"
        return response

# configure CS50 Library to use SQLite database
db = SQL("sqlite:///mashup.db")

@app.route("/")
def index():
    """Render map."""
    if not os.environ.get("API_KEY"):
        raise RuntimeError("API_KEY not set")
    return render_template("index.html", key=os.environ.get("API_KEY"))

@app.route("/articles")
def articles():
    """Look up articles for geo."""
    
    #get the value of geo from the url
    #output the article objects as json data
    return jsonify(lookup(request.args.get("geo")))

@app.route("/search")
def search():
    """Search for places that match query."""
    
    #retrieve everything the user typed without the coma
    args = [arg.strip() for arg in request.args.get("q").split(',')]
    
    #parameters for db execute
    cargs = {'place_name': '%', 'admin_name1': '%', 'postal_code': '%'}
    
    #loop to replace the values cargs from args
    for i in range(len(args)): 
            
        #change if args[i] is integer
        try:
            if cargs['postal_code'] == '%':
                cargs['postal_code'] = str(int(args[i])) + "%"
            
        #change if args[i] is otherwise, and assign it first to place name first
        except ValueError:
            
            #if place_name is None fill it
            if cargs['place_name'] == '%':
                cargs['place_name'] = args[i] + "%"
            
            #else fill admin_name1
            elif cargs['admin_name1'] == '%':
                cargs['admin_name1'] = args[i] + "%"
    
    #search from the databse with the new parameters
    places = db.execute("SELECT * FROM places WHERE place_name LIKE :pn AND admin_name1 LIKE :an1 AND postal_code LIKE :pc", 
                pn=cargs['place_name'], an1=cargs['admin_name1'], pc=cargs['postal_code'])
    
    #return the json data
    return jsonify(places)

@app.route("/update")
def update():
    """Find up to 10 places within view."""

    # ensure parameters are present
    if not request.args.get("sw"):
        raise RuntimeError("missing sw")
    if not request.args.get("ne"):
        raise RuntimeError("missing ne")

    # ensure parameters are in lat,lng format
    if not re.search("^-?\d+(?:\.\d+)?,-?\d+(?:\.\d+)?$", request.args.get("sw")):
        raise RuntimeError("invalid sw")
    if not re.search("^-?\d+(?:\.\d+)?,-?\d+(?:\.\d+)?$", request.args.get("ne")):
        raise RuntimeError("invalid ne")

    # explode southwest corner into two variables
    (sw_lat, sw_lng) = [float(s) for s in request.args.get("sw").split(",")]

    # explode northeast corner into two variables
    (ne_lat, ne_lng) = [float(s) for s in request.args.get("ne").split(",")]

    # find 10 cities within view, pseudorandomly chosen if more within view
    if (sw_lng <= ne_lng):

        # doesn't cross the antimeridian
        rows = db.execute("""SELECT * FROM places
            WHERE :sw_lat <= CAST(latitude AS NUMERIC) AND CAST(latitude AS NUMERIC) <= :ne_lat 
            AND (:sw_lng <= CAST(longitude AS NUMERIC) AND CAST(longitude AS NUMERIC) <= :ne_lng)
            GROUP BY country_code, place_name, admin_code1
            ORDER BY RANDOM()
            LIMIT 10""",
            sw_lat=sw_lat, ne_lat=ne_lat, sw_lng=sw_lng, ne_lng=ne_lng)

    else:

        # crosses the antimeridian
        rows = db.execute("""SELECT * FROM places
            WHERE :sw_lat <= CAST(latitude AS NUMERIC) AND CAST(latitude AS NUMERIC) <= :ne_lat 
            AND (:sw_lng <= CAST(longitude AS NUMERIC) OR CAST(longitude AS NUMERIC) <= :ne_lng)
            GROUP BY country_code, place_name, admin_code1
            ORDER BY RANDOM()
            LIMIT 10""",
            sw_lat=sw_lat, ne_lat=ne_lat, sw_lng=sw_lng, ne_lng=ne_lng)

    # output places as JSON
    return jsonify(rows)
