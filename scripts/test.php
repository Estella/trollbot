<?php
  /* bind(string network, string type, string flags, string mask, string function) */
  bind("severed","pub","-","hi","testing");

  function testing($nick, $uhost, $hand, $chan, $arg)
  {
      fdsjkfdslkjfdsklfsjsd
    putserv("severed","PRIVMSG #trollbot :triggered from PHP");
  }

?>
echo "test loaded";
