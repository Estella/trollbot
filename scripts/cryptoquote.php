<?php
  /*********************************************************************
   * Cryptoquote-1.0.0.php : A cryptoquote game written for trollbot   *
   *********************************************************************
   * Description:                                                      *
   *   This script was developed as part of a proof of concept for     *
   * TrollBot's PHP scripting ability. Now with OOP.                   *
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

  bind("DALnet",             /* Network */
       "pub",                /* Type    */
       "-",                  /* Flags   */
       "!cryptoquote",       /* Mask    */
       "start_cryptoquote"); /* Func    */

  class Cryptoquote
  {
    // Strings    
    var $quote;

    // Arrays
    var $game_scores; // $game_scores[$nick] = game score
    var $turn_scores; // $turn_scores[$nick] = turn score
    
    var $key;
    var $solved;
   
    // Boolean
    var $running;
    
    // ctor mofo
    function Cryptoquote($net, $chan)
    {
      $this->running = false;
      $this->net     = $net;
      $this->chan    = $chan
    }

    function start($net, $nick, $chan)
    {
      if ($running == true)
      {
        putserv($net,"PRIVMSG $chan :A cryptoquote game is already in play.");
        return;
      }

      
    }
    
  }

  function start_cryptoquote($net, $nick, $uhost, $hand, $chan, $text)
  {
    global $game;

    if (!isset($game[$net][$chan]))
      $game[$net][$chan] = new Cryptoquote();
    
    $game[$net][$chan]->start($net,$nick,$chan));
  }
?>
