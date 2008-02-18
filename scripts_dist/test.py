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

def callback(netw, nick, uhost, hand, chan, arg):
   trollbot.putserv("PRIVMSG %s :Python Support Enabled" % (chan))
   trollbot.putserv("PRIVMSG %s :GOT: %s, %s, %s, %s, %s, %s" % (chan,netw, nick, uhost, hand, chan, arg))

def load():
   result = trollbot.bind("pub", "-", "!python", "test.callback")

#end load
