<?php
   /**
    * Lookup functions in php manual
    * 
    * Requires a local, untarred copy of the manual (individual files)
    * Available at php.net
    **/
	ini_set("max_execution_time",0);

   define('MANUAL_BASE_DIR', '/home/comcor/src/phpman/html');

   function xform_function($function) {
      return str_replace("_", "-", $function);
   }

   function parse_func_data($file, $function) {
      $data = file_get_contents($file);
      $rdata = array(); 
      /* get function name */
      /* NB: the DOM parser chokes on the PHPDOC function HTML files, so we'll do
       * this the old fashioned way.*/
      if (preg_match_all('/class="refname">(.*?)<\/h1>.*?/', $data, $matches)) {
         $rdata['name'] = $matches[1][0];
      } 

      if (preg_match_all('/class="verinfo">(.*?)<\/p>/', $data, $matches)) {
         $rdata['version'] = $matches[1][0];
      }

      if (preg_match_all('/class="refpurpose">(.*?)<\/p>/', $data, $matches)) {
         $rdata['purpose'] = $matches[1][0];
      }

      /*
       <span class="methodparam"><span class="type">array</span> <tt class="parameter reference">&$array</tt></span>
       */
       if (preg_match_all("/class=\"methodsynopsis\">\n[\s]*<span class=\"type\">(.*?)<\/span>/", $data, $matches)) {
         $rdata['return_type'] = $matches[1][0];
       }

       if (preg_match_all('/class="methodparam"><span class="type">(.*?)<\/span>[\s]*<tt class="parameter reference">(.*?)<\/tt>/', $data, $matches)) {
         array_shift($matches);
         $fargs = array();
         for ($i = 0; $i < count($matches); $i+=2) {
            $fargs[] = $matches[$i][0].' '.$matches[$i+1][0];
         }
         $rdata['args'] = $fargs;
       }
      
      return (count($rdata) >= 4) ? $rdata : null;
   }

   function lookup($net, $nick, $uhost, $hand, $chan, $arg) {
      print "Lookup handler called";
      $function = $arg;
      $file = MANUAL_BASE_DIR.'/function.'.xform_function($function).'.html'; 
      if (file_exists($file)) {
         $data = parse_func_data($file, $function);
         $line1 = $data['return_type'].' '.$data['name'].'('.implode(",", $data['args']).')';
         $line2 = $data['version'].' '.str_replace($function, '', $data['purpose']);

         putserv($net, 'PRIVMSG '.$chan.' :'.$line1);
         putserv($net, 'PRIVMSG '.$chan.' :'.$line2);
         putserv($net, 'PRIVMSG '.$chan.' :'.'http://php.net/'.xform_function($function));
         print $line1."\n";
      } else {
         /* noop */
      }
   }

  
   bind("DALnet","pub","-","!php","lookup");

?>
