############################################################
# TrollBot v0.0.1 written by poutine                       #
############################################################

PHP Scripting:

  PHP scripting on TrollBot is different than the other 
scripting languages because PHP does not allow me to have 
multiple interpreters. It does everything in a global space
and frankly I think that's retarded. All gripes aside, here's
how to use TrollBot's PHP scripting ability which is very
similiar to eggdrop.

  First off we have to deal with the network parameter in
each function. There's no point or harm in loading a 
phpscript in different network brackets in the config file
but it will be loaded in a global namespace once, so binds 
and associated functions need to be multi-network aware.

  The scripting system consists of two elements. The bind,
which is a definition of an event to trigger on, and the
associated function. Here's an example.

bind("testnet",   /* Only works on testnet, can be * or all  */
     "pub",       /* Public Messages (explained in TYPES)    */
     "-|-",       /* Flags user triggering event must have   */
                  /* <glob_flags|channel_flags> - means none */
     "!hi",       /* mask, see TYPES for what this matches   */
     "somefunc"); /* Function to call when event triggers    */


  When someone says "!hi" (without the quotes stupid), 
somefunc() is called with the params specified in TYPES.
for pub, the params are network, nickname, userhost, handle, 
channel, arg. $arg is the IRC line minus the trigger and
trailing whitespace. Here's what somefunc() should look like

function somefunc($net, $nick, $uhost, $hand, $chan, $arg)
{
  putserv($net,"PRIVMSG $chan :!hi triggered by $nick");
}


  I don't see putserv in the PHP manual! That's because it's
added by me for use with IRC. If a person on network testnet
said the following line in the channel #trollbot:

<someweirdo> !hi this is a test

somefunc() would be called like this:

somefunc("testnet",
         "someweirdo",
         "someweirdo!ident@test.hostname.com",
         "someweirdo", /* Username */
         "#trollbot",
         "this is a test"); /* arg */



