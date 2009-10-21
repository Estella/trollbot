bind("pub","-","!javascript","do_javascript");
//bind("pub","-","!js","eval_js");
bind("pub","-","!vjs","do_version");
bind("pub","-","!ocjs","do_onchan");
bind("pub","-","!matchattr","do_matchattr");
bind("pub","-","!js-finduser","do_finduser");
bind("pub","-","!js-countusers","do_countusers");
bind("pub","-","!js-validuser","do_validuser");
bind("pub","-","!js-passwdok","do_passwdok");
bind("pub","-","!js-channels","do_channels");
bind("pub","-","!js-encpass","do_encpass");
bind("pub","-","!js-topic","do_topic");
bind("pub","-","!js-getchanmode", "do_getchanmode");
bind("pub","-","!js-isop", "do_isop");
bind("pub","-","!js-isvoice", "do_isvoice");
bind("pub","-","!js-die", "do_die");
bind("pub","-","!js", "do_eval");

function do_die(nick, uhost, hand, chan, arg)
{
	die(arg);
}

var bold = String.fromCharCode(2);

function do_isop(nick, uhost, hand, chan, arg)
{
	var args = arg.split(' ');
	if (args.length == 2)
	{
		var tnick  = args[0];
		var tchan  = args[1];
		var result = isop(tnick, tchan);
	}
	else
	{
		var tnick  = args[0];
		var result = isop(tnick);
	}

	putserv("PRIVMSG "+chan+" :"+bold+result+bold);
}

function do_isvoice(nick, uhost, hand, chan, arg)
{
	var args = arg.split(' ');
	if (args.length == 2)
	{
		var tnick  = args[0];
		var tchan  = args[1];
		var result = isvoice(tnick, tchan);
	}
	else
	{
		var tnick  = args[0];
		var result = isvoice(tnick);
	}

	putserv("PRIVMSG "+chan+" :"+bold+result+bold);
}


function do_getchanmode(nick, uhost, hand, chan, arg)
{
	var args = arg.split(' ');
	var tchan = args[0];

	putserv("PRIVMSG "+chan+" :Modes for ("+bold+tchan+bold+") are ("+bold+getchanmode(tchan)+bold+")");
}

function do_topic(nick, uhost, hand, chan, arg)
{
	var args = arg.split(' ');
	var tchan = args[0];
	
	putserv("PRIVMSG "+chan+" :Topic for ("+bold+tchan+bold+") is ("+bold+topic(tchan)+bold+")");
}

function do_encpass(nick, uhost, hand, chan, arg)
{
	putserv("PRIVMSG "+chan+" :Pass hash for ("+arg+") == "+encpass(arg));
}

function do_channels(nick, uhost, hand, chan, arg){
	var chans = channels();

	putserv("PRIVMSG "+chan+" :In channels: "+bold+chans+bold);
}

function do_passwdok(nick, uhost, hand, chan, arg){
	var args = arg.split(' ');
	var handle=args[0];
	var pass="";

	if (args.length == 2){
		pass=args[1];
	}

	if (passwdok(handle, pass)){
		if (pass==""){
			putserv("PRIVMSG "+chan+" :User has a password");
		}
		else {
			putserv("PRIVMSG "+chan+" :That password is correct");
		}
	}
	else {
		if (pass==""){
			putserv("PRIVMSG "+chan+" :User does not have a password");
		}
		else {
			putserv("PRIVMSG "+chan+" :That password is not correct");
		}
	}
}

function do_validuser(nick, uhost, hand, chan, arg){
	var ret = validuser(arg);

	if (ret){
		putserv("PRIVMSG "+ chan +" :User is valid.");
	}
	else {
		putserv("PRIVMSG "+ chan +" :User is not valid.");
	}
}

function do_countusers(nick, uhost, hand, chan, arg)
{
	putserv("PRIVMSG " + chan + " :There are currently (" + countusers() + ") users in the bot");
}

function do_finduser(nick, uhost, hand, chan, arg)
{
	putserv("PRIVMSG " + chan + " :" + finduser(arg));

}

function do_matchattr(nick, uhost, hand, chan, arg)
{
	var args = arg.split(' ');

	if (args.length == 3)
		putserv("PRIVMSG " + chan + " :" + matchattr(args[0],args[1],args[2]));
	else
		putserv("PRIVMSG " + chan + " :" + matchattr(args[0],args[1]));
}

function do_eval(nick, uhost, hand, chan, arg)
{
	var result = eval(arg);
	
	if (result != 'undefined')
		putserv("PRIVMSG " + chan + " :JS Result: " + result);
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
