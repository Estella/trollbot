<?php
	ini_set("max_execution_time",0);

  bind("DALnet","pub","-","!bmi","do_bmi");
  bind("severed","pub","-","!bmi","do_bmi");

  bind("DALnet","pub","-","!mbmi","do_mbmi");
  bind("severed","pub","-","!mbmi","do_mbmi");


  function weight_stat($bmi) 
  { 
    if ($bmi < 18.5) 
      return "underweight"; 
    else if ($bmi >= 18.5 && $bmi < 24.9) 
      return "normal"; 
    else if ($bmi > 25 && $bmi < 29.9) 
      return "overweight"; 
    else 
      return "a fat fucking piece of shit"; 
  }

  function do_bmi($net, $nick, $uhost, $hand, $chan, $arg)
  {
    $arrarg = explode(' ',$arg); 
   
    if (count($arrarg) < 2) 
    {
      putserv($net,"PRIVMSG $chan :!bmi <weight> [<feet><inches>][<total inches>]"); 
      return;
    }
   
    if (count($arrarg == 2)) 
      $inches = intval($arrarg[1]) * 12 + intval($arrarg[2]); 
    else 
      $inches = intval($arrarg[2]); 

    $bmi = (intval($arrarg[0]) / pow($inches,2)) *  703; 

    putserv($net,"PRIVMSG $chan :Your BMI is: (" . round($bmi,1) . ") which means you are (" . weight_stat($bmi) . ")");
  }

  function do_mbmi($net, $nick, $uhost, $hand, $chan, $arg)
  {
    $arrarg = explode(' ',$arg); 
  
    if (count($arrarg) != 2) 
    {
      putserv($net,"PRIVMSG $chan :!mbmi <weight in kilograms> <height in centimetres>"); 
      return;
    }

    $weight = intval($arrarg[0]); 
    $centimetres = intval($arrarg[1]); 
    $bmi = (intval($weight) / pow($centimetres,2)) *  10000; 
    
    putserv($net,"PRIVMSG $chan :$nick, Your BMI is: (" . round($bmi,1) . ") which means you are (" . weight_stat($bmi) . ")");
  }
?>
