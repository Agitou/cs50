from cs50 import SQL
from flask import Flask, flash, redirect, render_template, request, session, url_for
from flask_session import Session
from passlib.apps import custom_app_context as pwd_context
from tempfile import mkdtemp
from datetime import datetime

from helpers import *

# configure application
app = Flask(__name__)

# ensure responses aren't cached
if app.config["DEBUG"]:
    @app.after_request
    def after_request(response):
        response.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
        response.headers["Expires"] = 0
        response.headers["Pragma"] = "no-cache"
        return response

# custom filter
app.jinja_env.filters["usd"] = usd

# configure session to use filesystem (instead of signed cookies)
app.config["SESSION_FILE_DIR"] = mkdtemp()
app.config["SESSION_PERMANENT"] = False
app.config["SESSION_TYPE"] = "filesystem"
Session(app)

# configure CS50 Library to use SQLite database
db = SQL("sqlite:///finance.db")

@app.route("/")
@login_required
def index():
    
    #get all the portfolio accounts that the user owns
    port_ids = db.execute("SELECT portfolio_id from userPortfolio WHERE user_id=:user_id", user_id=session["user_id"])
    
    #get the rows with the user's portfolio ids
    rows = db.execute("SELECT symbol, name, total_shares, pps FROM portfolio JOIN stock ON portfolio.stock_id=stock.id")
    
    #get user's cash
    cash = db.execute("SELECT cash from users WHERE id=:id", id=session["user_id"])[0]["cash"]
    
    #format cash to two decimal points
    cash = float("{0:.2f}".format(cash))
    
    #return index with user's portfolio accounts
    return render_template("index.html", rows=rows, cash=cash)

@app.route("/buy", methods=["GET", "POST"])
@login_required
def buy():
    """Buy shares of stock."""
    
    #if request is POST
    if request.method == "POST":
        
        #store the number of shares
        sharesToBuy = int(request.form.get("shares"))
        
        #check if shares is correct
        if not sharesToBuy or sharesToBuy < 0:
            return apology("Invalid share amount")
            
        #check if symbol is valid
        stock = lookup(request.form.get("symbol"))
        
        if not stock:
            return apology("Invalid stock")
            
        #add it to stocks if the stock is not in the database
        if not db.execute("SELECT * FROM stock WHERE name=:name", name=stock["name"]):
            db.execute("INSERT INTO stock(name, symbol, pps) VALUES(:name, :symbol, :pps)",
                name=stock["name"], symbol=stock["symbol"], pps=stock["price"])
        
        #else just get id
        stockID = db.execute("SELECT id FROM stock WHERE name=:name", name=stock["name"])[0]["id"]
            
        #check if the user can afford the shares
        sharePrice = float(sharesToBuy * stock["price"])
        userCash = db.execute("SELECT cash FROM users WHERE id=:id", id=session["user_id"])[0]["cash"]
        
        #if the user can't, return apology 
        if sharePrice > userCash:
            return apology("Insufficient amount of cash")
            
        #get current user's portfolios
        portfolios = db.execute("SELECT * FROM userPortfolio WHERE user_id=:user_id", user_id=session["user_id"])
        edited = False
        
        #edit portfolio if it exists
        for i in range(len(portfolios)):
            if db.execute("SELECT stock_id from portfolio WHERE id=:pID", pID=portfolios[i]["portfolio_id"])[0]["stock_id"] == stockID:
                        
                db.execute("UPDATE portfolio SET total_shares = total_shares + :sharesToBuy WHERE stock_id=:stock_id",
                    sharesToBuy=sharesToBuy, stock_id=stockID)
                edited = True
                break
        
        #else create a new account   
        if not edited:
            
            #if he didn't, create a portfolio account
            portfolioID = db.execute("INSERT INTO portfolio (stock_id, total_shares) VALUES (:stock_id, :total_shares)",
                stock_id=stockID, total_shares=sharesToBuy)
            
            #and link it to the user
            db.execute("INSERT INTO userPortfolio (user_id, portfolio_id) VALUES (:user_id, :portfolio_id)",
                user_id=session["user_id"], portfolio_id=portfolioID)
                
        #update user's cash amount
        db.execute("UPDATE users SET cash=cash - :sharePrice WHERE id=:id", sharePrice=sharePrice, id=session["user_id"])
        
        #record current transaction history 
        currentDT = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
        db.execute("INSERT INTO transactions(item_id, user_id, quantity, time) VALUES(:stock_id, :user_id, :quantity, :date)",
            stock_id=stockID, user_id=session["user_id"], quantity=sharesToBuy, date=currentDT)
            
        #flash log in message
        flash("Bought!")
        
        #return index page
        return redirect(url_for("index"))
        
    #elif request is GET
    else:
        return render_template("buy.html")
        
@app.route("/history")
@login_required
def history():
    """Show history of transactions."""
    
    #get all the transactions of this user
    trans = db.execute("SELECT symbol, quantity, pps, time from transactions JOIN stock ON transactions.item_id=stock.id WHERE user_id=:user_id", user_id=session["user_id"])
    
    print(trans)
    #return index with user's portfolio accounts
    return render_template("history.html", rows=trans)

@app.route("/login", methods=["GET", "POST"])
def login():
    """Log user in."""

    # forget any user_id
    session.clear()

    # if user reached route via POST (as by submitting a form via POST)
    if request.method == "POST":

        # ensure username was submitted
        if not request.form.get("username"):
            return apology("must provide username")

        # ensure password was submitted
        elif not request.form.get("password"):
            return apology("must provide password")

        # query database for username
        rows = db.execute("SELECT * FROM users WHERE username = :username", username=request.form.get("username"))

        # ensure username exists and password is correct
        if len(rows) != 1 or not pwd_context.verify(request.form.get("password"), rows[0]["hash"]):
            return apology("invalid username and/or password")

        # remember which user has logged in
        session["user_id"] = rows[0]["id"]
        
        #flash log in message
        flash('You were successfully logged in')
        
        # redirect user to home page
        return redirect(url_for("index"))

    # else if user reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("login.html")

@app.route("/logout")
def logout():
    """Log user out."""

    # forget any user_id
    session.clear()

    # redirect user to login form
    return redirect(url_for("login"))

@app.route("/quote", methods=["GET", "POST"])
@login_required
def quote():
    """Get stock quote."""
    
    #if request is POST
    if request.method == "POST":
        
        #retrieve quote; return apology if symbol's invalid
        quote = lookup(request.form.get("symbol"))
        
        if not quote:
            return apology("Invalid Symbol")
            
        #if invalid return a page with the information
        return render_template("quoted.html", quote=quote)
        
    #elif request is GET
    else:
        return render_template("quote.html")

@app.route("/register", methods=["GET", "POST"])
def register():
    """Register user."""
    
    #if request is POST
    if request.method == "POST":
        
        #check if username was entered
        if not request.form.get("username"):
            return apology("must provide username")
            
        #check if password was entered
        elif not request.form.get("password"):
            return apology("must provide password")
            
        #check if confirmation password match the password
        elif request.form.get("password_confirm") != request.form.get("password"):
            return apology("passwords do not match")
        
        #add the user to the database, return apology if username already exists
        result = db.execute("INSERT INTO users(username, hash) VALUES (:username, :hash)", 
            username=request.form.get("username"), 
            hash=pwd_context.encrypt(request.form.get("password")))
            
        if not result:
            return apology("account with the username already exists")
            
        #forget any user_id
        session.clear()
        
        #log the user in
        session["user_id"] = result
        
        #redirect user to home page
        return redirect(url_for("index"))
        
    #elif request is GET
    else:
        return render_template("register.html")

@app.route("/sell", methods=["GET", "POST"])
@login_required
def sell():
    """Sell shares of stock."""
    
    #if method is post
    if request.method == "POST":
        
        #return apology if symbol or shares is incorrect
        symbol = lookup(request.form.get("symbol"))
        
        if not symbol or not request.form.get("shares") or int(request.form.get("shares")) < 0 :
            return apology("Invalid symbol/share")
        
        #else store shares to sell
        sharesToSell = int(request.form.get("shares"))
        
        #check if the user has portfolio account with this share
        portfolios = db.execute("SELECT * FROM userPortfolio WHERE user_id=:user_id", user_id=session["user_id"])
        sold = False
        
        #edit portfolio if it exists
        for i in range(len(portfolios)):
            
            #stockID from the current row of the portfolios
            stockID = db.execute("SELECT stock_id FROM portfolio WHERE id=:id", id=portfolios[i]["portfolio_id"])[0]["stock_id"]
            
            #if stock exists, try to sell
            if db.execute("SELECT symbol from stock WHERE id=:id", id=stockID)[0]["symbol"] == symbol["symbol"]:
                
                #check if shares to sell isn't more than the user's shares
                if sharesToSell > db.execute("SELECT total_shares from portfolio WHERE id=:id", id=portfolios[i]["portfolio_id"])[0]["total_shares"]:
                    
                    #return apology when it's more
                    return apology("Invalid share amount")
                
                #else update everything    
                else:
                    #update portfolio amount
                    db.execute("UPDATE portfolio set total_shares = total_shares - :sharesToSell WHERE id=:id", id=portfolios[i]["portfolio_id"], sharesToSell=sharesToSell)        
                    
                    sharePrice = sharesToSell * symbol["price"]
                    #update user's cash amount
                    db.execute("UPDATE users SET cash = cash + :sharePrice WHERE id=:id", sharePrice=sharePrice, id=session["user_id"])
                
                    #record current transaction history 
                    currentDT = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
                    db.execute("INSERT INTO transactions(item_id, user_id, quantity, time) VALUES(:stock_id, :user_id, :quantity, :date)",
                        stock_id=stockID, user_id=session["user_id"], quantity=(-sharesToSell), date=currentDT) 
                    
                    sold = True
                
                    #flash log in message
                    flash("Sold!")
                    break
        
        if not sold: 
            #if sold is not true after the loop ends, then portfolio account doesn't exist
            return apology("Illegal symbol")
        else:
            #return index page
            return redirect(url_for("index"))
    
    #elif if method is get
    else:
        return render_template("sell.html")