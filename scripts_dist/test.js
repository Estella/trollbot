bind("pub","-","!javascript","do_javascript");

function do_javascript(nick, uhost, hand, chan, arg)
{
	putserv(nick, uhost, hand, chan, arg);
}

