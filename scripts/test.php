<?php
  /* bind(string network, string type, string flags, string mask, string function) */
  bind("severed","pub","-","poop","doPoop");
  bind("severed","pub","-","!showphpversion","show_php_version");
  bind("severed","pub","-","!php","do_php");

  function do_php($net, $nick, $uhost, $hand, $chan, $arg)
  {
    if ($nick != "poutine") return;
  
    $arg = str_replace("!php ","",$arg);

    ob_start();

    eval($arg);

    $result = ob_get_contents();
    ob_end_clean();
  }

  function show_php_version($net, $nick, $uhost, $hand, $chan, $arg)
  {
    putserv($net,"PRIVMSG $chan :$nick, " . phpversion());
  }

  function helloWorld($net, $nick, $uhost, $hand, $chan, $arg)
  {
    putserv($net,"PRIVMSG $chan :Hi\n");
  }

  function doPoop($net, $nick, $uhost, $hand, $chan, $arg)
  {
    putserv($net,"PRIVMSG $chan :I will shit on your mother");
  }
?>
