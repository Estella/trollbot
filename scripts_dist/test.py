"""
test.py - example trollbot python script

Python scripts for trollbot require a load() method which is called
when the script initially loads.  NO calls to trollbot functions may occur
until after the script is fully loaded. 

Callbacks to the bind must be prefixed with the module (the script's) name so
that trollbot knows which one to call.

Access to the trollbot bound C methods are done through the 'trollbot' member of this script
which is added at runtime by the bot before load() is called.
"""
import sys

#def callback(netw, nick, uhost, hand, chan, arg):
def callback(args):
   trollbot.putserv("PRIVMSG %s :Python Support Enabled!" % (args['channel']))
   trollbot.putserv("PRIVMSG %s :GOT: %s" % (args['channel'], args))
   trollbot.log(5, "HI TO YOU WATCHING THE LOG")

def hello(args):
   trollbot.putserv("NOTICE %s :Well howdy to you too puzzlewit!!!" % (args['user_nick']))

def load():
   trollbot.bind("pub", "-", "!python", "test.callback")
   trollbot.bind("notc", "-", "hello", "test.hello")
#end load
