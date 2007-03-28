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
       "start_cryptoquote"); /* Func    */



  /* Script Portion */
  function start_cryptoquote($net, $nick, $uhost, $hand, $chan, $text)
  {
    global $quotes_file, $scores_file;

    if (!file_exists($quotes_file))
    {
      putserv($net,"PRIVMSG $chan :Could not open $quotes_file");
      return;
    }

    $lines = file($quotes_file);

    $quote = $lines[rand(0,count($lines))];

    putserv($net,"PRIVMSG $chan :Cryptoquote starting");
    putserv($net,"PRIVMSG $chan :$quote");
  }

  function show_cryptoquote($net, $nick, $uhost, $hand, $chan, $text)
  {
  }
?>
