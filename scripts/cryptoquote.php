<?php
  /*********************************************************************
   * Cryptoquote-1.0.0.php : A cryptoquote game written for trollbot   *
   *********************************************************************
   * Description:                                                      *
   *   This script was developed as part of a proof of concept for     *
   * TrollBot's PHP scripting ability.                                 *
   *********************************************************************
   * Triggers:                                                         *
   *   pub:                                                            *
   *     !cryptoquote - Starts a a new game                            *
   *     !showquote   - Shows a quote if a game is running             *
   *   pubm:                                                           *
   *     a-z*         - Catches greedy guesses                         *
   *********************************************************************
   * egg_lib usage:                                                    *
   *   putserv                                                         *
   *   bind                                                            *
   *********************************************************************/

  ini_set('display_errors',1);
  ini_set('error_reporting','E_ALL');

  /* Configuration */
   
  /* Where is the scores file stored */
  $scores_file = "text/cryptoscores.txt";
  
  /* Where is the quotes file stored */
  $quotes_file = "text/quotes.txt";


  /* Initialize the triggers            */
  bind("severed",            /* Network */
       "pub",                /* Type    */
       "-",                  /* Flags   */
       "!cryptoquote",       /* Mask    */
       "start_cryptoquote"); /* Func    */

  bind("severed",            /* Network */
       "pub",                /* Type    */
       "-",                  /* Flags   */
       "!showquote",         /* Mask    */
       "show_cryptoquote");  /* Func    */



  /* Script Portion */
  function start_cryptoquote($net, $nick, $uhost, $hand, $chan, $text)
  {
    global $quotes_file, $scores_file, $game;

    if (isset($game[$net][$chan]['started']))
    {
      if ($game[$net][$chan]['started'] == 1)
      {
        putserv($net,"PRIVMSG $chan :Cryptoquote game is already running dumbass");
        return;
      }
    } 

    if (!file_exists($quotes_file))
    {
      putserv($net,"PRIVMSG $chan :Could not open $quotes_file");
      return;
    }

    $lines = file($quotes_file);

    $game[$net][$chan]['quote'] = strtolower($lines[rand(0,count($lines))]);
    
    $arr = str_split($game[$net][$chan]['quote']);

    $arr = array_unique($arr);
 
    $str = implode("",$arr);

    $str = preg_replace("/[^a-z]/","",$str);

    $alf = str_split("abcdefghijklmnopqrstuvwxyz");

    shuffle($alf);

//    for($i = count($alf) - count($str);$i>0;$i--)
//      array_pop($alf);

    putserv($net,"PRIVMSG $chan :Cryptoquote starting");
    putserv($net,"PRIVMSG $chan :" . $str . " " . $alf);
    
    $game[$net][$chan]['started'] = 1;
  }

  function show_cryptoquote($net, $nick, $uhost, $hand, $chan, $text)
  {
    global $game;

    if (isset($game[$net][$chan]['started']))
    {
      if ($game[$net][$chan]['started'] == 0)
      {
        putserv($net,"PRIVMSG $chan :Cryptoquote game isn't running dumbshit");
        return;
      }
    }

    putserv($net,"PRIVMSG $chan :" . $game[$net][$chan]['quote']);

  }
?>
