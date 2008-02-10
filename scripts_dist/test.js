bind("pub","-","!javascript","do_javascript");
bind("pub","-","!js","eval_js");

function eval_js(nick, uhost, hand, chan, arg)
{
	js_eval(arg,chan);
}

function do_javascript(nick, uhost, hand, chan, arg)
{
	putserv("PRIVMSG " + chan + " :Nick: " + nick + " Uhost: " + uhost + " Chan: " + chan + " Arg: " + arg);
}

