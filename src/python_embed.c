#include <Python.h>
#include "main.h"
#include "python_embed.h"

#include "irc_trigger.h"
#include "irc.h"
#include "network.h"
#include "server.h"
#include "channel.h"
#include "user.h"
#include "config_engine.h"
#include "python_lib.h"
#include "log_entry.h"

#include "egg_lib.h"

#define PY_INTERNAL_CORE 1

#ifdef PY_INTERNAL_CORE
#define PY_CORE_LIB "__main__"
#else
#define PY_CORE_LIB "tb_py_core"
#endif
#define PY_NET_INIT_METH "__TB_init_network"
#define PY_NET_LOAD_MODULE_METH "__TB_load_module"
#define PY_NET_CALL_METH "__TB_call_method"
#define PY_NET_ADD_PATH_METH "__TB_add_path"
#define TROLLBOT_BINDING_MODULE "trollbot"
#define PY_STR(a) PyString_FromString(a)
#define PY_DICT_ADD(a, b, c) PyDict_SetItemString(a, b, PyString_FromString(c))

#ifndef HAVE_SIGHANDLER_T
typedef void (*sighandler_t)(int);
#endif /* HAVE_SIGHANDLER_T */

/* Exported functions to Python */
PyMethodDef PyTbMethods[] = {
	{"unbind", py_unbind, METH_VARARGS,
		"Unbinds a previously set bind"},

	{"savechannels", py_savechannels, METH_VARARGS,
	  "Saves channel file"},

	{"validuser", py_validuser, METH_VARARGS,
		"Checks if a user is valid"},

	{"bind", py_bind, METH_VARARGS,
		"binds an IRC event to python function"},

	{"putserv", py_putserv, METH_VARARGS,
		"Sends the text to the network."},

	{"log", py_troll_debug, METH_VARARGS, 
		"Logs a message to the bot log."},

	{"privmsg", py_privmsg, METH_VARARGS, 
		"Sends a private message to a channel or user"},
	{NULL, NULL, 0, NULL}
};

	void cfg_init_python(struct config *cfg) {
		if (cfg->py_main != NULL)
			return;

		sighandler_t t_sig;
		t_sig = signal(SIGINT, SIG_IGN);

		/* initialize the python interpreter */
		Py_Initialize();
		troll_debug(LOG_DEBUG, "[python] Initialized interpreter");
		/* restore the signal handler */
		signal(SIGINT, t_sig);
		troll_debug(LOG_DEBUG, "[python] Restored signal handler");
		bootstrap_binding_module();
		troll_debug(LOG_DEBUG, "[python] Loaded trollbot binding module");

#ifdef PY_INTERNAL_CORE
		int ret;
		ret = PyRun_SimpleString(python_core_module_code);
		if (!ret) {
			troll_debug(LOG_DEBUG, "[python] Loaded internal core module");
		} else {
			troll_debug(LOG_DEBUG, "[python] An error occurred while loading internal core module");
		}
#else
		load_python_module(PY_CORE_LIB);
#endif
	}

void bootstrap_binding_module() {
	PyObject * module;

	/* initialize the module which exposes to python
	 * our wrappers for the bots functionality we want to expose
	 * NOTE: all exposable methods are defined in the PyTbMethods
	 */

	module = Py_InitModule(TROLLBOT_BINDING_MODULE, PyTbMethods);

	/* bind constants to our module */
	PyModule_AddIntConstant(module, "LOG_DEBUG", LOG_DEBUG);
	PyModule_AddIntConstant(module, "LOG_ALL", LOG_ALL);
	PyModule_AddIntConstant(module, "LOG_WARN", LOG_WARN);
	PyModule_AddIntConstant(module, "LOG_ERROR", LOG_ERROR);
	PyModule_AddIntConstant(module, "LOG_FATAL", LOG_FATAL);
	PyModule_AddIntConstant(module, "LOG_NONE", LOG_NONE);

	PyModule_AddStringConstant(module, "TRIG_PUB", "pub");
	PyModule_AddStringConstant(module, "TRIG_PUBM", "pubm");
	PyModule_AddStringConstant(module, "TRIG_MSG", "msg");
	PyModule_AddStringConstant(module, "TRIG_MSGM", "msgm");
	PyModule_AddStringConstant(module, "TRIG_SIGN", "sign");
	PyModule_AddStringConstant(module, "TRIG_PART", "part");
	PyModule_AddStringConstant(module, "TRIG_JOIN", "join");
	PyModule_AddStringConstant(module, "TRIG_NOTC", "notc");
	/* PyModule_AddStringConstant(module, "TRIG_FIL", "fil"); */
	PyModule_AddStringConstant(module, "TRIG_DCC", "dcc");
	PyModule_AddStringConstant(module, "TRIG_RAW", "raw");
	PyModule_AddStringConstant(module, "TRIG_KICK", "kick");
	PyModule_AddStringConstant(module, "TRIG_TOPC", "topc");

}

void net_init_python(struct config *cfg, struct network *net) {

	PyObject * args[1];
	PyObject * rval;

	troll_debug(LOG_DEBUG, "[python] initializing network context %s",net->label);

	net->py_netobj = PyCObject_FromVoidPtr((void*)net, NULL);
	Py_XINCREF(net->py_netobj);
	args[0] = net->py_netobj;

	rval = call_python_method(PY_CORE_LIB, PY_NET_INIT_METH, args, 1);
}

void python_load_scripts_from_config(struct config *cfg) {

	int i;
	struct network *net = cfg->networks;

	while (net != NULL) {
		if (net->py_scripts != NULL) {
			for (i=0; net->py_scripts[i] != NULL; i++) {
				load_python_net_module(net, net->py_scripts[i]);
			}
		}
		net = net->next;
	}
}

/**
 * loads a python module for the provided network
 */
void load_python_net_module(struct network *net, char * filename) {

	PyObject * args[2];
	PyObject *rval;

	troll_debug(LOG_DEBUG, "[python] loading module `%s' for network %s", filename, net->label);

	Py_XINCREF(net->py_netobj);
	args[0] = net->py_netobj;
	args[1] = PyString_FromString(filename);

	rval = call_python_method(PY_CORE_LIB, PY_NET_LOAD_MODULE_METH, args, 2);
}

/**
 * load a python module from its module name
 */
void load_python_module(char *filename) {

	PyObject *pyModuleName;
	PyObject *pyModule;

	pyModuleName = PyString_FromString(filename);
	pyModule = PyImport_Import(pyModuleName);
	if (pyModule != NULL) {
		troll_debug(LOG_DEBUG, "[python] Core Module (%s) loaded", filename);
		/* Py_DECREF(pyModule); */
	} else {
		if (PyErr_Occurred()) 
			PyErr_Print();
		troll_debug(LOG_DEBUG, "[python] Failed to load Core Module (%s)", filename);
	}
	Py_DECREF(pyModuleName);
}

/**
 * Call a python method via namesapce
 */
PyObject * call_python_method(char * module, char * method, PyObject ** args, int numargs) {

	int i;
	PyObject *module_py;
	PyObject *method_py;
	PyObject *args_py;
	PyObject *rval_py;

	/* get reference to module containing method */
	module_py = PyImport_AddModule(module);
	if (module_py == NULL) {
		troll_debug(LOG_DEBUG, "[python] Could not locate module: %s", module);
		Py_RETURN_FALSE;
	}

	/* get reference to method in module */
	method_py = PyObject_GetAttrString(module_py, method);
	if (method_py == NULL) {
		troll_debug(LOG_DEBUG, "[python] Could not locate method %s in module %s",module, method);
		Py_RETURN_FALSE;
	}

	if (PyCallable_Check(method_py)) {
		/* create argument tuple */
		args_py = PyTuple_New(numargs);
		for (i = 0; i < numargs; i++) {
			PyTuple_SetItem(args_py, i, args[i]);
		}

		rval_py = PyObject_CallObject(method_py, args_py);

		Py_DECREF(args_py);
		Py_DECREF(method_py);
		/* Py_DECREF(module_py); */

		if (rval_py == NULL) {
			troll_debug(LOG_DEBUG, "[python] an error occurred calling %s.%s()", module, method);
			if (PyErr_Occurred()) {
				PyErr_Print();
			}
			Py_RETURN_FALSE;
		} else {
			return rval_py;
		}

	} else {
		troll_debug(LOG_DEBUG, "[python] %s.%s is not callable", method, module);
		Py_RETURN_FALSE;
	}
}


/**
 * Execute a python callback on the given network
 */
/* FIXME: Treating troll_makearg's output as non-static memory */
void py_handler(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf) {
	PyObject *args[3];
	PyObject *rval;
	PyObject *dict;

	//build a dict to contain our args
	dict = PyDict_New();

	Py_XINCREF(net->py_netobj);
	//we always need these for the call_python_method call
	args[0] = net->py_netobj;
	args[1] = PY_STR(trig->command);
	args[2] = dict;
	PY_DICT_ADD(dict, "network", net->label);
	PY_DICT_ADD(dict, "handle", "*");

	troll_debug(LOG_DEBUG, "[python] py_handler called for command %s", trig->command);

	/**
	 * Triggers
	 *  - PUB: calls the callback method with the arg string stripped of the leading trigger
	 *         args: network, nick, host, *, channel, trigger args*
	 *  - PUBM: calls the callback method with the entire arg string
	 *         args: network, nick, host, *, channel, entire trigger line
	 *
	 */
	switch (trig->type) {
		/*
		 * (4) PUB
		 *  bind pub <flags> <command> <proc>
		 *  procname <nick> <user@host> <handle> <channel> <text>
		 */
		case TRIG_PUB:
			PY_DICT_ADD(dict, "user_nick", data->prefix->nick);
			PY_DICT_ADD(dict, "user_host", data->prefix->host);
			PY_DICT_ADD(dict, "channel", data->c_params[0]);
			PY_DICT_ADD(dict, "text", troll_makearg(data->rest_str, trig->mask));

			rval = call_python_method(PY_CORE_LIB, PY_NET_CALL_METH, args, 3);
			troll_debug(LOG_DEBUG, "[python] executed TRIG_PUB callback");
			break; 
			/*
			 * (6) PUBM
			 *  bind pubm <flags> <mask> <proc>
			 *  procname <nick> <user@host> <handle> <channel> <text>
			 */
		case TRIG_PUBM:
			PY_DICT_ADD(dict, "user_nick", data->prefix->nick);
			PY_DICT_ADD(dict, "user_nick", data->prefix->nick);
			PY_DICT_ADD(dict, "user_host", data->prefix->host);
			PY_DICT_ADD(dict, "channel", data->c_params[0]);
			PY_DICT_ADD(dict, "text", data->rest_str);

			rval = call_python_method(PY_CORE_LIB, PY_NET_CALL_METH, args, 3);
			troll_debug(LOG_DEBUG, "[python] executed TRIG_PUBM callback");
			break;

			/*
			 * (1)  MSG
			 *  bind msg <flags> <command> <proc>
			 *  procname <nick> <user@host> <handle> <text>
			 */
		case TRIG_TOPC:
			PY_DICT_ADD(dict, "user_nick", data->prefix->nick);
			PY_DICT_ADD(dict, "user_nick", data->prefix->nick);
			PY_DICT_ADD(dict, "user_host", data->prefix->host);
			PY_DICT_ADD(dict, "channel", data->c_params[0]);
			PY_DICT_ADD(dict, "text", data->rest_str);

			rval = call_python_method(PY_CORE_LIB, PY_NET_CALL_METH, args, 3);
			troll_debug(LOG_DEBUG, "[python] executed TRIG_TOPC callback");
			break;
		case TRIG_MSG:
			PY_DICT_ADD(dict, "user_nick", data->prefix->nick);
			PY_DICT_ADD(dict, "user_nick", data->prefix->nick);
			PY_DICT_ADD(dict, "text", troll_makearg(data->rest_str, trig->mask));
			rval = call_python_method(PY_CORE_LIB, PY_NET_CALL_METH, args, 3);
			troll_debug(LOG_DEBUG, "[python] executed TRIG_MSG callback");
			break;
			/*
			 * (5) MSGM
			 *   bind msgm <flags> <mask> <proc>
			 *   procname <nick> <user@host> <handle> <text>
			 */
		case TRIG_MSGM:
			PY_DICT_ADD(dict, "user_nick", data->prefix->nick);
			PY_DICT_ADD(dict, "user_nick", data->prefix->nick);
			PY_DICT_ADD(dict, "text", data->rest_str);
			rval = call_python_method(PY_CORE_LIB, PY_NET_CALL_METH, args, 3);
			troll_debug(LOG_DEBUG, "[python] executed TRIG_MSGM callback");
			break;

			/* (2) DCC
			 *  bind dcc <flags> <command> <proc>
			 *  procname <handle> <idx> <text>
			 */
		case TRIG_DCC:
			break;

			/* (3) FILE
			 * bind fil <flags> <command> <proc>
			 * procname <handle> <idx> <text>
			 case TRIG_FIL:
			 break;
			 */

			/*
			 * (7)  NOTC (stackable)
			 *   bind notc <flags> <mask> <proc>
			 *   procname <nick> <user@host> <handle> <text> <dest>
			 */
		case TRIG_NOTC:
			PY_DICT_ADD(dict, "user_nick", data->prefix->nick);
			PY_DICT_ADD(dict, "user_host", data->prefix->host);
			PY_DICT_ADD(dict, "text", data->rest_str);
			PY_DICT_ADD(dict, "dest", data->c_params[0]);
			rval = call_python_method(PY_CORE_LIB, PY_NET_CALL_METH, args, 3);
			troll_debug(LOG_DEBUG, "[python] executed TRIG_MSGM callback");
			break;

			/*
			 * (8)  JOIN (stackable)
			 *   bind join <flags> <mask> <proc>
			 *   procname <nick> <user@host> <handle> <channel>
			 */
		case TRIG_JOIN:
			break;

			/*
			 * (9)  PART (stackable)
			 *   bind part <flags> <mask> <proc>
			 *   procname <nick> <user@host> <handle> <channel> <msg>
			 */
		case TRIG_PART:
			break;

			/*
			 * (10) SIGN (stackable)
			 *   bind sign <flags> <mask> <proc>
			 *   procname <nick> <user@host> <handle> <channel> <reason>
			 */
		case TRIG_SIGN:
			break;

			/*
			 * (11) TOPC (stackable)
			 *   bind topc <flags> <mask> <proc>
			 *   procname <nick> <user@host> <handle> <channel> <topic>
			 *
			 *   Description: triggered by a topic change. mask can contain wildcards
			 *     and is matched against '#channel <new topic>'.
			 *   Module: irc
			 */

			/*
			 * (12) KICK (stackable)
			 *   bind kick <flags> <mask> <proc>
			 *   procname <nick> <user@host> <handle> <channel> <target> <reason>
			 *
			 *   Description: triggered when someone is kicked off the channel. The
			 *     mask is matched against '#channel target reason' where the target is
			 *     the nickname of the person who got kicked (can contain wildcards).
			 *     The proc is called with the nick, user@host, and handle of the
			 *     kicker, plus the channel, the nickname of the person who was
			 *     kicked, and the reason.
			 *   Module: irc
			 */
			/*
			 * (13) NICK (stackable)
			 bind nick <flags> <mask> <proc>
			 procname <nick> <user@host> <handle> <channel> <newnick>

Description: triggered when someone changes nicknames. The mask
is matched against '#channel newnick' and can contain wildcards.
Channel is "*" if the user isn't on a channel (usually the bot not
yet in a channel).
Module: irc
*/
			/*
			 * (14) MODE (stackable)
			 bind mode <flags> <mask> <proc>
			 proc-name <nick> <user@host> <handle> <channel> <mode-change> <target>

Description: mode changes are broken down into their component
parts before being sent here, so the <mode-change> will always
be a single mode, such as "+m" or "-o". target will show the
argument of the mode change (for o/v/b/e/I) or "" if the set
mode does not take an argument. The bot's automatic response
to a mode change will happen AFTER all matching Tcl procs are
called. The mask will be matched against '#channel +/-modes'
and can contain wildcards.

If it is a server mode, nick will be "", user@host is the server
name, and handle is *.

Note that "target" was added in 1.3.17 and that this will break
Tcl scripts that were written for pre-1.3.17 Eggdrop that use the
mode binding. Also, due to a typo, mode binds were broken
completely in 1.3.17 but were fixed in 1.3.18. Mode bindings are
not triggered at all in 1.3.17.

One easy example (from guppy) of how to support the "target"
parameter in 1.3.18 and later and still remain compatible with
older Eggdrop versions is:

Old script looks as follows:
bind mode - * mode_proc
proc mode_proc {nick uhost hand chan mode} { ... }

To make it work with 1.3.18+ and stay compatible with older bots, do:
bind mode - * mode_proc_fix
proc mode_proc_fix {nick uhost hand chan mode {target ""}} {
if {$target != ""} {append mode " $target"}
mode_proc $nick $uhost $hand $chan $mode
}
proc mode_proc {nick uhost hand chan mode} { ... }
Module: irc
*/
			/*
			 * (15) CTCP (stackable)
			 bind ctcp <flags> <keyword> <proc>
			 proc-name <nick> <user@host> <handle> <dest> <keyword> <text>

Description: dest will be a nickname (the bot's nickname, obviously)
or channel name. keyword is the ctcp command (which can contain
wildcards), and text may be empty. If the proc returns 0, the bot
will attempt its own processing of the ctcp command.
Module: server
*/
			/*
			 * (16) CTCR (stackable)
			 bind ctcr <flags> <keyword> <proc>
			 proc-name <nick> <user@host> <handle> <dest> <keyword> <text>

Description: just like ctcp, but this is triggered for a ctcp-reply
(ctcp embedded in a notice instead of a privmsg)
Module: server
*/
			/*
			 * (17) RAW (stackable)
			 bind raw <flags> <keyword> <proc>
			 procname <from> <keyword> <text>

Description: previous versions of Eggdrop required a special compile
option to enable this binding, but it's now standard. The keyword
is either a numeric, like "368", or a keyword, such as "PRIVMSG".
from will be the server name or the source user (depending on
the keyword); flags are ignored. The order of the arguments is
identical to the order that the IRC server sends to the bot. The
pre-processing  only splits it apart enough to determine the
keyword. If the proc returns 1, Eggdrop will not process the line
any further (this could cause unexpected behavior in some cases).
Module: server
*/
			/*
			 * (18) BOT
			 bind bot <flags> <command> <proc>
			 proc-name <from-bot> <command> <text>

Description: triggered by a message coming from another bot in
the botnet. The first word is the command and the rest becomes
the text argument; flags are ignored.
Module: core
*/
			/*
			 * (19) CHON (stackable)
			 bind chon <flags> <mask> <proc>
			 proc-name <handle> <idx>

Description: when someone first enters the party-line area of the
bot via dcc chat or telnet, this is triggered before they are
connected to a chat channel (so, yes, you can change the channel
in a 'chon' proc). mask is matched against the handle and supports
wildcards. This is NOT triggered when someone returns from the
file area, etc.
Module: core
*/
			/*
			 * (20) CHOF (stackable)
			 bind chof <flags> <mask> <proc>
			 proc-name <handle> <idx>

Description: triggered when someone leaves the party line to
disconnect from the bot. mask is matched against the handle and
can contain wildcards. Note that the connection may have already
been dropped by the user, so don't send output to the idx.
Module: core
*/
			/*
			 * (21) SENT (stackable)
			 bind sent <flags> <mask> <proc>
			 proc-name <handle> <nick> <path/to/file>

Description: after a user has successfully downloaded a file from
the bot, this binding is triggered. mask is matched against the
handle of the user that initiated the transfer and supports
wildcards. nick is the actual recipient (on IRC) of the file. The
path is relative to the dcc directory (unless the file transfer
was started by a script call to 'dccsend', in which case the path
is the exact path given in the call to 'dccsend').
Module: transfer
*/
			/*
			 * (22) RCVD (stackable)
			 bind rcvd <flags> <mask> <proc>
			 proc-name <handle> <nick> <path/to/file>

Description: triggered after a user uploads a file successfully.
mask is matched against the user's handle. nick is the IRC
nickname that the file transfer originated from. The path is
where the file ended up, relative to the dcc directory (usually
this is your incoming dir).
Module: transfer
*/
			/*
			 * (23) CHAT (stackable)
			 bind chat <flags> <mask> <proc>
			 proc-name <handle> <channel#> <text>

Description: when a user says something on the botnet, it invokes
this binding. Flags are ignored; handle could be a user on this
bot ("DronePup") or on another bot ("Eden@Wilde") and therefore
you can't rely on a local user record. The mask is checked against
the entire line of text and supports wildcards.

NOTE: If a BOT says something on the botnet, the BCST bind is
invoked instead.
Module: core
*/
			/*
			 * (24) LINK (stackable)
			 bind link <flags> <mask> <proc>
			 proc-name <botname> <via>

Description: triggered when a bot links into the botnet. botname
is the botnetnick of the bot that just linked in; via is the bot
it linked through. The mask is checked against the botnetnick of
the bot that linked and supports wildcards. flags are ignored.
Module: core
*/
			/*
			 * (25) DISC (stackable)
			 bind disc <flags> <mask> <proc>
			 proc-name <botname>

Description: triggered when a bot disconnects from the botnet for
whatever reason. Just like the link bind, flags are ignored; mask
is matched against the botnetnick of the bot that unlinked.
Wildcards are supported in mask.
Module: core
*/
			/*
			 * (26) SPLT (stackable)
			 bind splt <flags> <mask> <proc>
			 procname <nick> <user@host> <handle> <channel>

Description: triggered when someone gets netsplit on the channel.
Be aware that this may be a false alarm (it's easy to fake a
netsplit signoff message on some networks); mask may contain
wildcards and is matched against '#channel nick!user@host'.
Anyone who is SPLT will trigger a REJN or SIGN within the next
wait-split (defined in the config file) minutes.
Module: irc
*/
			/*
			 * (27) REJN (stackable)
			 bind rejn <flags> <mask> <proc>
			 procname <nick> <user@host> <handle> <channel>

Description: someone who was split has rejoined. mask can contain
wildcards, and is matched against '#channel nick!user@host'.
Module: irc
*/
			/*
			 * (28) FILT (stackable)
			 bind filt <flags> <mask> <proc>
			 procname <idx> <text>

Description: party line and file system users have their text sent
through filt before being processed. If the proc returns a blank
string, the text is considered parsed. Otherwise, the bot will use
the text returned from the proc and continue parsing that
Module: core
*/
			/*
			 * (29) NEED (stackable)
			 bind need <flags> <mask> <proc>
			 procname <channel> <type>

Description: this bind is triggered on certain events, like when
the bot needs operator status or the key for a channel. The
types are: op, unban, invite, limit, and key; the mask is
matched against '#channel type' and can contain wildcards. flags
are ignored.

Example:
bind need - "% op" needop < handles only need op
bind need - "*" needall   < handles all needs
Module: irc
*/
			/*
			 * (30) FLUD (stackable)
			 bind flud <flags> <type> <proc>
			 procname <nick> <user@host> <handle> <type> <channel>

Description: any floods detected through the flood control settings
(like 'flood-ctcp') are sent here before processing. If the proc
returns 1, no further action is taken on the flood; if the proc
returns 0, the bot will do its normal "punishment" for the flood.
The flood types are: pub, msg, join, or ctcp (and can be masked to
"*" for the bind); flags are ignored.
Module: server
*/
			/*
			 * (31) NOTE (stackable)
			 bind note <flags> <mask> <proc>
			 procname <from> <to> <text>

Description: incoming notes (either from the party line, someone on
IRC, or someone on another bot on the botnet) are checked against
these binds before being processed. The mask is matched against
the receiving handle and supports wildcards. If the proc returns 1,
Eggdrop will not process the note any further. Flags are ignored.
Module: core
*/
			/*
			 * (32) ACT (stackable)
			 bind act <flags> <mask> <proc>
			 proc-name <handle> <channel#> <action>

Description: when someone does an action on the botnet, it invokes
this binding. flags are ignored; the mask is matched against the
text of the action and can support wildcards.
Module: core
*/
			/*
			 * (33) WALL (stackable)
			 bind wall <flags> <mask> <proc>
			 proc-name <from> <msg>

Description: when the bot receives a wallops, it invokes this
binding. flags are ignored; the mask is matched against the text
of the wallops msg. Note that RFC shows the server name as a source
of the message, whereas many IRCds send the nick!user@host of the
actual sender, thus, Eggdrop will not parse it at all, but simply
pass it to bind in its original form. If the proc returns 1,
Eggdrop will not log the message that triggered this bind.
Module: server
*/
			/*
			 * (34) BCST (stackable)
			 bind bcst <flags> <mask> <proc>
			 proc-name <botname> <channel#> <text>

Description: when a bot broadcasts something on the botnet (see
'dccbroadcast' above), it invokes this binding. flags are ignored;
the mask is matched against the message text and can contain
wildcards. 'channel' argument will always be '-1' since broadcasts
are not directed to any partyline channel.

It is also invoked when a BOT (not a person, as with the CHAT bind)
'says' something on a channel. In this case, the 'channel' argument
will be a valid channel, and not '-1'.
Module: core
*/
			/*
			 * (35) CHJN (stackable)
			 bind chjn <flags> <mask> <proc>
			 proc-name <botname> <handle> <channel#> <flag> <idx> <user@host>

Description: when someone joins a botnet channel, it invokes this
binding. The mask is matched against the channel and can contain
wildcards. flag is one of: * (owner), + (master), @ (op), or %
(botnet master). Flags are ignored.
Module: core
*/
			/*
			 * (36) CHPT (stackable)
			 bind chpt <flags> <mask> <proc>
			 proc-name <botname> <handle> <idx> <channel#>

Description: when someone parts a botnet channel, it invokes this
binding. The mask is matched against the channel and can contain
wildcards. Flags are ignored.
Module: core
*/
			/*
			 * (37) TIME (stackable)
			 bind time <flags> <mask> <proc>
			 proc-name <minute> <hour> <day> <month> <year>

Description: allows you to schedule procedure calls at certain
times. mask matches 5 space separated integers of the form:
"minute hour day month year". minute, hour, day, month have a
zero padding so they are exactly two characters long; year is
four characters. Flags are ignored.
Module: core
*/
			/*
			 * (38) AWAY (stackable)
			 bind away <flags> <mask> <proc>
			 proc-name <botname> <idx> <text>

Description: triggers when a user goes away or comes back on the
botnet. text is the reason than has been specified (text is ""
when returning). mask is matched against the botnet-nick of the
bot the user is connected to and supports wildcards. flags are
ignored.
Module: core
*/
			/*
			 * (39) LOAD (stackable)
			 bind load <flags> <mask> <proc>
			 proc-name <module>

Description: triggers when a module is loaded. mask is matched
against the name of the loaded module and supports wildcards;
flags are ignored.
Module: core
*/
			/*
			 * (40) UNLD (stackable)
			 bind unld <flags> <mask> <proc>
			 proc-name <module>

Description: triggers when a module is unloaded. mask is matched
against the name of the unloaded module and supports wildcards;
flags are ignored.
Module: core
*/
			/*
			 * (41) NKCH (stackable)
			 bind nkch <flags> <mask> <proc>
			 proc-name <oldhandle> <newhandle>

Description: triggered whenever a local user's handle is changed
(in the userfile). mask is matched against the user's old handle
and can contain wildcards; flags are ignored.
Module: core
*/
			/*
			 * (42) EVNT (stackable)
			 bind evnt <flags> <type> <proc>
			 proc-name <type>

Description: triggered whenever one of these events happen. flags
are ignored; valid events are:
sighup          - called on a kill -HUP <pid>
sigterm         - called on a kill -TERM <pid>
sigill          - called on a kill -ILL <pid>
sigquit         - called on a kill -QUIT <pid>
save            - called when the userfile is saved
rehash          - called just after a rehash
prerehash       - called just before a rehash
prerestart      - called just before a restart
logfile         - called when the logs are switched daily
loaded          - called when the bot is done loading
userfile-loaded - called after userfile has been loaded

connect-server    - called just before we connect to an IRC server
init-server       - called when we actually get on our IRC server
disconnect-server - called when we disconnect from our IRC server
Module: core
*/
			/*
			 * (43) LOST (stackable)
			 bind lost <flags> <mask> <proc>
			 proc-name <handle> <nick> <path> <bytes-transferred> <length-of-file>

Description: triggered when a DCC SEND transfer gets lost, such as
when the connection is terminated before all data was successfully
sent/received. This is typically caused by a user abort.
Module: transfer
*/
			/*
			 * (44) TOUT (stackable)
			 bind tout <flags> <mask> <proc>
			 proc-name <handle> <nick> <path> <bytes-transferred> <length-of-file>

Description: triggered when a DCC SEND transfer times out. This may
either happen because the dcc connection was not accepted or
because the data transfer stalled for some reason.
Module: transfer
*/
			}
}


/**
 * adds a string path to the interpreter's sys.path list
 */
void python_add_path(char * pathname) {
	PyObject * args[1];
	PyObject * rval;

	rval = call_python_method(PY_CORE_LIB, PY_NET_ADD_PATH_METH, args, 1);
}

/* Oops, sorry about the formatting [acidtoken] */
#ifdef PY_INTERNAL_CORE
char * python_core_module_code = 
"import imp\n"
"import sys\n"
"import trollbot\n"
"import types\n"
"\n"
"NETWORKS = {}\n"
"\n"
"#TrollbotNetworkInterface\n"
"#Marshals access to the trollbot module for the given network.\n"
"#Each script loaded will have a reference to an instance of this\n"
"#object for the network it is attached to\n"
"\n"
"class TrollbotNetworkInterface:\n"
"\n"
"   def __init__(self, network):\n"
"      self.network = network #ptr to TB net struct\n"
"      self.modules = {}\n"
"   #end __init__\n"
"\n"
"   def load_module(self, name):\n"
"      ok = False \n"
"      if (name.find(\"/\") != -1):\n"
"         comps = name.split(\"/\")\n"
"         path = \"/\".join(comps[:-1])\n"
"         name = comps[-1]\n"
"         if path not in sys.path:\n"
"            sys.path.insert(0,path)\n"
"      #end path handling\n"
"\n"
"      #strip .py off\n"
"      if (name[-3:] == \".py\"):\n"
"         name = name[:-3]\n"
"\n"
"      fp, pathname, description = imp.find_module(name)\n"
"\n"
"      try:\n"
"         module = imp.load_module(name, fp, pathname, description)\n"
"         if module is not None:\n"
"            #pass module a reference to us\n"
"            module.trollbot = self\n"
"            #initialize our module\n"
"            if getattr(module, \"load\") != None:\n"
"               module.load()\n"
"            else:\n"
"               raise \"Missing load() method for module!\"\n"
"            #save the module for access later\n"
"            self.modules[name] = module\n"
"            ok = True\n"
"      finally:\n"
"         if fp:\n"
"            fp.close\n"
"      return ok\n"
"   #end load_module\n"
"\n"
"   def __getattr__(self, name):\n"
"       member = getattr(trollbot, name,None)\n"
"       if member == None:\n"
"          trollbot.log(self.network, 3, \"could not find member: %s\" % (name))\n"
"          return None\n"
"       if type(member) == types.BuiltinFunctionType:\n"
"          #closure wraps the method for our purposes\n"
"          def _tb_method_wrapper(*args):\n"
"             return member(self.network, *args)\n"
"          return _tb_method_wrapper\n"
"       else:\n"
"          return member\n"
"\n"
"   def call_method(self,callback, *args):\n"
"      #get module name\n"
"      (module, method) = callback.split(\".\")\n"
"      if self.modules.has_key(module):\n"
"         cb = getattr(self.modules[module], method)\n"
"         cb(*args)\n"
"\n"
"#end TrollbotNetworkInterface\n"
"\n"
"\n"
"##\n"
"## INTERNAL FUNCTIONS\n"
"##\n"
"## Please don't call these unless you know what you are doing.\n"
"## These are meant to be called through the Python C/API\n"
"##\n"
"\n"
"def __TB_init_network(network):\n"
"   if not NETWORKS.has_key(network):\n"
"      NETWORKS[network] = (TrollbotNetworkInterface(network))\n"
"#end init_network\n"
"\n"
"def __TB_load_module(network, module):\n"
"   if NETWORKS.has_key(network):\n"
"      return NETWORKS[network].load_module(module)\n"
"     #end if\n"
"   return False\n"
"#end __TB_load_module\n"
"\n"
"def __TB_call_method(network, callback, *args):\n"
"   if NETWORKS.has_key(network):\n"
"      return NETWORKS[network].call_method(callback, *args)\n"
"   return False\n"
"#end __TB_call_method\n"
"\n"
"def __TB_add_path(path):\n"
"   sys.path.append(path)\n"
"#end __TB_add_path\n"
"\n"
"def __TB_reload_module(modname):\n"
"   if sys.modules.has_key(modname):\n"
"      reload(sys.modules[modname])\n"
"      reload(sys.modules[modname])\n"
"      print \"reloaded %s \\n\" % (modname)\n"
"      return True\n"
"   print \"found no module named %s to reload\\n\" % (modname)\n"
"   return False\n"
"#end __TB_reload_module\n"
"\n"
"\n";
#endif

/* vim: tabstop=2
*/
