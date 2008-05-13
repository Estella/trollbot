<?php
	ini_set("max_execution_time",0);

  /* bind(string network, string type, string flags, string mask, string function) */
  bind("DALnet","pub","-","hi","testing");
  bind("severed","dcc","-",".help","do_help");
  bind("DALnet","pub","-",".version","do_version");

  function do_version($net, $nick, $uhost, $hand, $chan, $arg)
  {
    putserv($net,"PRIVMSG $chan :" . phpversion());
  }

  function do_help($handle, $idx, $text)
  {
    putserv($net, "PRIVMSG poutine :this is a test");
  }

  function testing($net, $nick, $uhost, $hand, $chan, $arg)
  {
    putserv($net,"PRIVMSG $chan :triggered from PHP: net=$net nick=$nick uhost=$uhost hand=$hand chan=$chan arg=($arg)");
  }

?>
echo "test loaded";
