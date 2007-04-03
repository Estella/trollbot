<?php
  /* bind(string network, string type, string flags, string mask, string function) */
  bind("severed","pub","-","!poo","doTest");
  bind("severed","pubm","-","*fuck*","no_cuss");
  bind("severed","msg","-","!test","msgtest");
  bind("severed","msgm","-","*willis*","willis");
  bind("DALnet","pub","-","!matchtest","matchtest");
  
  function matchtest($net, $nick, $uhost, $hand, $arg)
  {
    $arr = explode(" ",$arg);

    $ret = matchwild($arr[0],$arr[1]);

    putserv($net, "PRIVMSG $chan :Result: $ret");
  }
  
  function willis($net, $nick, $uhost, $hand, $text)
  {
    putserv($net, "PRIVMSG $nick :whatcha talking bout nigger");
  }

  function msgtest($net, $nick, $uhost, $hand, $arg)
  {
    putserv($net, "PRIVMSG #trollbot :$nick, hey fucker no cussing");
  }

  function doTest($net, $nick, $uhost, $hand, $chan, $arg)
  {
    putserv($net, "PRIVMSG $chan :wee" . ini_get('display_errors'));
  }

  function no_cuss($net, $nick, $uhost, $hand, $chan, $arg)
  {
    putserv($net, "PRIVMSG $chan :$nick, hey fucker no cussing");
    return 1;
  }
?>
