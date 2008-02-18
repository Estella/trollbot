bind("pub","-","!javascript","do_javascript");
bind("pub","-","!js","eval_js");
bind("pub","-","!vjs","do_version");
bind("pub","-","!ocjs","do_onchan");
bind("pub","-","!matchattr","do_matchattr");

function do_matchattr(nick, uhost, hand, chan, arg)
{
	var args = arg.split(' ');

	if (args.length == 3)
		putserv("PRIVMSG " + chan + " :" + matchattr(args[0],args[1],args[2]));
	else
		putserv("PRIVMSG " + chan + " :" + matchattr(args[0],args[1]));
}


function eval_js(nick, uhost, hand, chan, arg)
{
	js_eval(arg,chan);
}

function do_javascript(nick, uhost, hand, chan, arg)
{
	putserv("PRIVMSG " + chan + " :Nick: " + nick + " Uhost: " + uhost + " Chan: " + chan + " Arg: " + arg);
}

function do_version(nick, uhost, hand, chan, arg){
	putserv("PRIVMSG "+ chan +" :"+botname+" is "+version);
}

function do_onchan(nick, uhost, hand, chan, arg){
	var args=arg.split(' ');
	var nickCheck=args[0];
	var channelCheck = args[1];
	
	var result = onchan(nickCheck, channelCheck);
	if (result){
		putserv("PRIVMSG "+ chan +" :"+nickCheck+" is on");
	}
	else {
		putserv("PRIVMSG "+ chan +" :"+nickCheck+" is not currently on");
	}
}
