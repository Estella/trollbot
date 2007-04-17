<?php
  /* bind(string network, string type, string flags, string mask, string function) */
  bind("severed","pub","-","hi","testing");
  bind("severed","dcc","-",".help","do_help");

  function do_help($net, $handle, $idx, $text)
  {
    putserv($net, "PRIVMSG poutine :this is a test");
  }

  function testing($net, $nick, $uhost, $hand, $chan, $arg)
  {
    putserv("severed","PRIVMSG #trollbot :triggered from PHP");
  }

?>
echo "test loaded";
