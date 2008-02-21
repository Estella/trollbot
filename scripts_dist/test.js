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

var bold = String.fromCharCode(2);

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

/*
function eval_js(nick, uhost, hand, chan, arg)
{
	js_eval(arg,chan);
}*/

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
