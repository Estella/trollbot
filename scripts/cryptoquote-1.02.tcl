#############################################################
# Cryptoquote v1.02                                         #
#############################################################
#   This is a cryptoquote game which can run in multiple    #
# channels simultaneously. It's quite fun and aims to be as #
# configurable as possible.                                 #
#############################################################
# written by poutine/DALnet #tcl 2006-10-06 - 2006-10-07    #
#############################################################
# THIS SOURCE IS FREE FOR ANY USE. ORIGINAL AUTHOR MUST BE  #
# CREDITED IN SOURCE IN ANY DERIVED WORKS.                  #
#############################################################


#################
# Configuration #
#################


## GENERAL CONFIG (YOU MUST EDIT THIS) ##

# Set this to the channels you want this game to run in.
# Separate the list of channels with spaces
set crypto_chans "#tcl #fascism #cryptoquote #christian_debate #php #drinkers"

# Set this to where you want the scores file to be stored
set crypto_scores_file "text/cryptoscores.txt"

# Set this to the quotes file
set crypto_quotes_file "text/quotes.txt"


## IMPORTANT TO LOOK AT BUT NOT MANDATORY ##

# Set this to enable greedy guesses
# When this is set to 1, you can use x=y instead of !guess x=y, this 
# may suck up CPU, disable it if you're having speed problems.
set greedy_guesses 1

# Set this to 1 to enable !solve
# Some people may not like it, who knows
set enable_solve 1

# Set this to 1 to enable !cryptohint
set hints_enabled 1

# Set this to the number of hints the bot will give
# Setting it to 26 makes it unlimited (26 letters in the alphabet duh)
set number_hints 5

# If greater than this percentage of the quote is revealed, don't give a
# hint. Set to 100 for this to be disabled.
set hint_solved_percent 50

## PREFERENCES (OPTIONAL) ##

# If you wish to have your unsolved characters bold, underline, or reversed
# set this to the ASCII code you with to use.
# \003<number> == color
# \002         == bold
# \x1f         == underline
# \x16         == reverse
set unsolved_encoding ""

# If you wish to have your solved characters bold, underline, or reversed
# set this to the ASCII code you with to use.
# \003<number> == color
# \002         == bold
# \x1f         == underline
# \x16         == reverse
set solved_encoding "\x02"



## CODE BELOW (DO NOT EDIT UNLESS YOU KNOW TCL, DONT ASK FOR SUPPORT IF YOU DO! ##


# The following globals are used and not defined above
# $a_chan_in_game($chan)         = array: 0, off; 1, in game
# $a_chan_quote($chan)           = array: The channel's decoded quote
# $a_chan_crypt_quote($chan)     = array: The channel's encoded quote
# $l_chan_solved($chan)          = list:  solved chars for a channel in [array get] form
# $l_chan_decode($chan)          = list:  decoding tables for a channel in [array get] form
# $l_chan_games_scores($chan)    = list:  game scores for a channel in [array get] form
# $l_chan_player_scores($chan)   = list:  persistant scores for the players in a channel, in [array get] form

## BIND TABLE ##
bind pub - "!solve"        cryptoquote:solve
bind pub - "!cryptoquote"  cryptoquote:get_quote
bind pub - "!showquote"    cryptoquote:show_quote
bind pub - "!cryptoscore"  cryptoquote:show_score
bind pub - "!cryptoscores" cryptoquote:show_scores
bind pub - "!cryptohint"   cryptoquote:crypto_hint
bind pub - "!cryptostop"   cryptoquote:crypto_stop
bind pub - "!guess"        cryptoquote:guess

# If greedy guesses are turned on, bind an entry for every letter=
if {$greedy_guesses == 1} {
  bind pubm - "*a=*" cryptoquote:guess
  bind pubm - "*b=*" cryptoquote:guess
  bind pubm - "*c=*" cryptoquote:guess
  bind pubm - "*d=*" cryptoquote:guess
  bind pubm - "*e=*" cryptoquote:guess
  bind pubm - "*f=*" cryptoquote:guess
  bind pubm - "*g=*" cryptoquote:guess
  bind pubm - "*h=*" cryptoquote:guess
  bind pubm - "*i=*" cryptoquote:guess
  bind pubm - "*j=*" cryptoquote:guess
  bind pubm - "*k=*" cryptoquote:guess
  bind pubm - "*l=*" cryptoquote:guess
  bind pubm - "*m=*" cryptoquote:guess
  bind pubm - "*n=*" cryptoquote:guess
  bind pubm - "*o=*" cryptoquote:guess
  bind pubm - "*p=*" cryptoquote:guess
  bind pubm - "*q=*" cryptoquote:guess
  bind pubm - "*r=*" cryptoquote:guess
  bind pubm - "*s=*" cryptoquote:guess
  bind pubm - "*t=*" cryptoquote:guess
  bind pubm - "*u=*" cryptoquote:guess
  bind pubm - "*v=*" cryptoquote:guess
  bind pubm - "*w=*" cryptoquote:guess
  bind pubm - "*x=*" cryptoquote:guess
  bind pubm - "*y=*" cryptoquote:guess
  bind pubm - "*z=*" cryptoquote:guess
}

proc cryptoquote:solve { nick uhost hand chan arg } {
  global a_chan_in_game a_chan_quote a_chan_crypt_quote l_chan_solved l_chan_game_scores enable_solve

  if {$enable_solve == 0} { return }
  if {![info exists a_chan_in_game($chan)]} { return }
  if {$a_chan_in_game($chan) == 0} { return }

  set arg [string tolower $arg] 

  # $a_chan_quote($chan)
  # $a_chan_crypt_quote($chan)
  # Should ALWAYS be set here

  # Set all 
  set quote_chars   [regsub -all {[^a-zA-Z]+} $a_chan_quote($chan) ""]
  set crypted_chars [regsub -all {[^a-zA-Z]+} $a_chan_crypt_quote($chan) ""]
  set arg_chars     [regsub -all {[^a-zA-Z]+} $arg ""]

  # If any solved letters currently exist, copy them to a local array  
  if {[info exists l_chan_solved($chan)]} { array set solved $l_chan_solved($chan) }

  if {$quote_chars == $arg_chars} {

    # if l_chan_game_scores($chan) exists, copy to local array
    if {[info exists l_chan_game_scores($chan)]} { array set game_scores $l_chan_game_scores($chan) }

    set points 0

    # If they've never gotten a point this round, give them a 0 zero score
    if {![info exists game_scores($nick)]} { set game_scores($nick) 0 }

    # Go through all the chars and add up the ones which haven't been guessed
    for {set i 0} {$i < [string length $quote_chars]} {incr i} {

      # Only increase score if it hasn't been solved yet
      if {![info exists solved([string index $crypted_chars $i])]} {
        incr points
     
        set solved([string index $crypted_chars $i]) [string index $quote_chars $i]
      }

    }

    # Copy the local solved array back into the global one for the channel
    set l_chan_solved($chan) [array get solved]
        
    # Increase their round scores by the count of unsolved characters
    set game_scores($nick) [expr {$game_scores($nick) + $points}]


    set l_chan_game_scores($chan) [array get game_scores]

    putserv "PRIVMSG $chan :$nick solved the cryptoquote adding $points to his game score"

    cryptoquote:show_quote "" "" "" $chan ""

  }
}

proc cryptoquote:read_scores { } {
  global l_chan_player_scores crypto_scores_file

  set crypto_scores_fd [open $crypto_scores_file r]

  set file_scores [split [read $crypto_scores_fd] "\n"]
  close $crypto_scores_fd

  # Load all the scores in
  foreach {line} $file_scores {
    set chan  [lindex $line 0]
    set nick  [lindex $line 1]
    set score [lindex $line 2]
 
    if {[info exists l_chan_player_scores($chan)]} { 
      set l_chan_player_scores($chan) "$l_chan_player_scores($chan) $nick $score"
    } else {
      set l_chan_player_scores($chan) "$nick $score"
    }
  }
}

proc cryptoquote:write_scores { } {
  global l_chan_player_scores crypto_scores_file
 
  set crypto_scores_fd [open $crypto_scores_file w]

  foreach {chan list} [array get l_chan_player_scores] {
    foreach {nick score} $list {
      puts $crypto_scores_fd "$chan $nick $score"
    }
  }

  close $crypto_scores_fd
}

proc cryptoquote:crypto_stop { nick uhost hand chan arg } {
  global a_chan_in_game

  if {![info exists a_chan_in_game($chan)]} { return }
  if {$a_chan_in_game($chan) == 0} { return }

  set a_chan_in_game($chan) 0

  putserv "PRIVMSG $chan :Cryptoquote game stopped by $nick"
}

proc cryptoquote:crypto_hint { nick uhost hand chan arg } {
  global a_chan_in_game hints_enabled number_hints a_chan_quote hint_solved_percent a_chan_quote a_chan_crypt_quote l_chan_solved l_chan_decode a_chan_number_hints

  if {![info exists a_chan_in_game($chan)]} { return }
  if {$a_chan_in_game($chan) == 0} { return }

  if {$hints_enabled == 0} { 
    putserv "PRIVMSG $chan :Hints are not enabled." 
    return
  }

  if {$a_chan_number_hints($chan) == 0} { 
    putserv "PRIVMSG $chan :There are no more hints available."
    return
  }

  set quote_chars   [regsub -all {[^a-zA-Z]+} $a_chan_quote($chan) ""]
  set crypted_chars [regsub -all {[^a-zA-Z]+} $a_chan_crypt_quote($chan) ""]

  if {[info exists l_chan_solved($chan)]} { array set solved $l_chan_solved($chan) }

  set solved_found   0
  set unsolved_found 0

  for { set i 0 } { $i < [string length $quote_chars] } { incr i } {
    if {[info exists solved([string index $crypted_chars $i])]} {
      incr solved_found
    } else {
      incr unsolved_found
      append unsolved [string index $quote_chars $i]
    }
  }

  set percent [expr {( double($solved_found) / double([string length $quote_chars]) ) * 100.00 }]
  
  if {$percent > $hint_solved_percent} {
    putserv "PRIVMSG $chan :Quote is more than ${hint_solved_percent}% complete, hints disabled"
    return
  }

  # Guaranteed to be set 
  array set decode $l_chan_decode($chan)

  set rand_hint [string index $unsolved [rand [string length $unsolved]]]

  if {![info exists l_chan_solved($chan)]} {
    set l_chan_solved($chan) "$decode($rand_hint) $rand_hint"
  } else {
    set l_chan_solved($chan) "$l_chan_solved($chan) $decode($rand_hint) $rand_hint"
  }

  putserv "PRIVMSG $chan :HINT: $decode($rand_hint) = $rand_hint"

  set a_chan_number_hints($chan) [expr { $a_chan_number_hints($chan) - 1 }]

  cryptoquote:show_quote "" "" "" $chan ""
}


proc cryptoquote:show_scores { nick uhost hand chan arg } {
  global l_chan_player_scores
 
  if {![info exists l_chan_player_scores($chan)]} {
    putserv "PRIVMSG $chan :Nobody has played cryptoquote yet!"
    return
  }

  # Set local array player_scores
  array set player_scores $l_chan_player_scores($chan)

  foreach {k v} [array get player_scores] {
    lappend newlist "$k $v"
  }

  set newlist [lsort -decreasing -index 1 -integer $newlist]

  set i 1
  foreach {k} $newlist {  
    if {$i == 11} { break }

    set both [split $k " "]
    append outputstr ", $i. [lindex $both 0] ([lindex $both 1])"
    incr i
  }

  set outputstr [string replace $outputstr 0 1]
  
  putserv "PRIVMSG $chan :Top 10: $outputstr"
}
  

proc cryptoquote:show_score { nick uhost hand chan arg } {
  global l_chan_player_scores

  if {![info exists l_chan_player_scores($chan)]} {
    putserv "PRIVMSG $chan :Nobody has played cryptoquote yet!"
    return
  }

  # Set local array player_scores
  array set player_scores $l_chan_player_scores($chan)

  if {[llength $arg] == 1} {
    set target [lindex $arg 0]
  } else {
    set target $nick
  }

  foreach {k v} [array get player_scores] {
    if {$target == $k} {
      putserv "PRIVMSG $chan :$k ($v)"
      return
    } 
  }

  putserv "PRIVMSG $chan :$target not found in scores"

}


# make an encoded word for a channel
proc cryptoquote:cryptize { chan } {
  global a_chan_quote a_chan_crypt_quote l_chan_decode

  set alphabet "a b c d e f g h i j k l m n o p q r s t u v w x y z"
  set remain   "a b c d e f g h i j k l m n o p q r s t u v w x y z"

  foreach letter $alphabet {    
    set randrem [rand [llength $remain]]
   
    # Try not to have a=a, etc, but don't lock up the system over it
    for { set i 0 } { $i < 5 } { incr i } {
      if {$letter == [lindex $remain $randrem]} {
        set randrem [rand [llength $remain]]
      } else {
        break;
      }
    }
        
    set decode_key($letter) [lindex $remain $randrem]
 
    set remain [lreplace $remain $randrem $randrem]
  }

  set l_chan_decode($chan) [array get decode_key]

  # code has been made, now to encode the quote
  set a_chan_crypt_quote($chan) [string map [array get decode_key] $a_chan_quote($chan)]

  return
}     
    
proc cryptoquote:get_quote { nick uhost hand chan arg } {
  global crypto_chans a_chan_in_game l_chan_game_scores l_chan_solved l_quotes a_chan_quote greedy_guesses enable_solve a_chan_number_hints number_hints

  # Check whether we operate in this channel
  if {[lsearch $crypto_chans $chan] == -1} return

  # if a game has been played in that channel before, and they're in game, don't continue
  if {[info exists a_chan_in_game($chan)]} {
    if {$a_chan_in_game($chan) == 1} {
      putserv "PRIVMSG $chan :A cryptoquote game is already in play, type !showquote to see it"
      return
    }
  }

  # Reset this game's scores
  if {[info exists l_chan_game_scores($chan)]} { unset l_chan_game_scores($chan) }
   
  # Reset the solved chars
  if {[info exists l_chan_solved($chan)]} { unset l_chan_solved($chan) }

  # Set the channel specific quote
  set emptyquote 1
  while {$emptyquote} {
    set a_chan_quote($chan) [string tolower [lindex $l_quotes [rand [llength $l_quotes]]]]
  
    if {[regexp -all {[a-zA-Z]*} $a_chan_quote($chan)] > 0} {
      set emptyquote 0
    }
  }
     
  set a_chan_number_hints($chan) $number_hints

  # encode the quote
  cryptoquote:cryptize $chan

  # we're now in a game
  set a_chan_in_game($chan) 1

  set output    "PRIVMSG $chan :$nick has started a cryptoquote game. Each lowercase letter stands for "
  append output "another. Guess the which letters stand for which by typing "

  if {$greedy_guesses == 1} {
    append output "!guess "
  }

  append output "letter=letter, multiple letters can be used. "

  if {$enable_solve == 1} {
    append output "You can solve the quote with !solve <quote>. Example: xyz=lot means x=l, y=o, z=t"
  }

  putserv $output

  cryptoquote:show_quote " " " " " " $chan " "

  return
}

proc cryptoquote:guess { nick uhost hand chan arg } {
  global a_chan_in_game l_chan_decode l_chan_solved l_chan_game_scores

  if {![info exists a_chan_in_game($chan)]} { return }
  if {$a_chan_in_game($chan) == 0} { return }
 
  if {[llength $arg] == 0 || [llength $arg] == 2 || [llength $arg] > 3} { return }

  if {[llength $arg] == 1} {
    set both [split $arg =]
 
    if {[llength $both] != 2} { return }

    set lettersl [lindex $both 0]
    set lettersr [lindex $both 1]
  }

  if {[llength $arg] == 3} {
    set lettersl [lindex $arg 0]
    set lettersr [lindex $arg 2]
  }

  # Improper syntax
  if {[string length $lettersl] != [string length $lettersr] || [string length $lettersl] == 0} { return }

  set points 0
 
  array set decode_key $l_chan_decode($chan)

  # Needs reset afterwards
  if {[info exists l_chan_solved($chan)]} { array set cur_solved $l_chan_solved($chan) }

  for {set i 0} {$i < [string length $lettersl]} {incr i} {
    if {[info exists decode_key([string index $lettersr $i])]} {
      if {$decode_key([string index $lettersr $i]) == [string index $lettersl $i]} {
        # We got a match, see if it's already solved
        if {![info exists cur_solved([string index $lettersl $i])]} {
          set match 1
          incr points
          set cur_solved([string index $lettersl $i]) [string index $lettersr $i]
          set local_solved([string index $lettersl $i]) [string index $lettersr $i]
        }
      }
    }
  }

  # Undo the old and put in the new
  set l_chan_solved($chan) [array get cur_solved]

  # if channel_gamescores($chan) exists, copy the array to game_scores
  if {[info exists l_chan_game_scores($chan)]} { array set game_points $l_chan_game_scores($chan) }

  # If we found at least one match
  if {[info exists match]} {  
    # If scores exist
    if {![info exists game_points($nick)]} {
      set game_points($nick) $points
    } else {
      set game_points($nick) [expr { $game_points($nick) + $points }]
    }

    set decoded ""
    foreach {k v} [array get local_solved] {
      append decoded ",$k=$v"
    }

    set decoded [string replace $decoded 0 0]
   
    putserv "PRIVMSG $chan :$points points for $nick! $decoded ( $game_points($nick) total )"

    # Reset game points
    set l_chan_game_scores($chan) [array get game_points]
    
    cryptoquote:show_quote "" "" "" $chan ""
  }

}

proc cryptoquote:show_quote { nick uhost hand chan arg } {
  global a_chan_in_game l_chan_solved a_chan_crypt_quote a_chan_quote unsolved_encoding solved_encoding
  
  if {![info exists a_chan_in_game($chan)]} { return }
  if {$a_chan_in_game($chan) == 0} { return }

  set quote ""

  set matches 0

  if {[info exists l_chan_solved($chan)]} { array set cur_solved $l_chan_solved($chan) }

  # 0 = unquoted
  # 1 = unsolved
  # 2 = solved
  set last_state 0

  foreach letter [split $a_chan_crypt_quote($chan) ""] {
    set curr_state 0
    if {[info exists cur_solved($letter)]} {
      incr matches
      set curr_state 2
    } else {
      if {[regexp -all {[a-z]} $letter] != 0} {
        set curr_state 1
      }
    }

    if {$curr_state != $last_state} {
      if {$last_state == 0} {
        if {$curr_state == 1} {
          append quote $unsolved_encoding
        } else {
          append quote $solved_encoding
        }
      } elseif {$curr_state == 0} {
        append quote "\017"
      } else {
        if {$curr_state == 1} {
          append quote "\017${unsolved_encoding}"
        } else {
          append quote "\017${solved_encoding}"
        }
      }
    }

    if {$curr_state == 2} {
      append quote [string toupper $cur_solved($letter)]
    } else {
      append quote $letter
    }

    set last_state $curr_state
  }

#  foreach letter [split $a_chan_crypt_quote($chan) ""] {
#    if {[info exists cur_solved($letter)]} {
#      incr matches
#      append quote [string toupper "${solved_encoding}$cur_solved($letter)\017"]
#    } else {
#      if {[regexp -all {[a-z]} $letter] != 0} { 
#        append quote "${unsolved_encoding}$letter\017"
#      } else {
#        append quote $letter
#      }
#    }
#  }

  set decoded_letters ""
  foreach {k v} [array get cur_solved] {
    lappend decoded_letters ", $k=$v"
  }
 
  set decoded_letters [string replace [join $decoded_letters] 0 1]

  putserv "PRIVMSG $chan :Cryptoquote: $quote"

  if {$matches == [string length [regsub -all {[^a-zA-Z]+} $a_chan_quote($chan) ""]]} {
    putserv "PRIVMSG $chan :Cryptoquote Solved!"

    cryptoquote:end_game $chan
  }

}

proc cryptoquote:end_game { chan } {
  global a_chan_in_game l_chan_game_scores l_chan_player_scores
    
  set win1 ""
  set win2 ""
  set win3 ""

  set winpoints1 0
  set winpoints2 0
  set winpoints3 0

  if {![info exists l_chan_game_scores($chan)]} {
    putserv "PRIVMSG $chan :Nobody wins! cheaters.."
    set a_chan_in_game($chan) 0
    return
  }

  array set game_points $l_chan_game_scores($chan)

  for {set search [array startsearch game_points]} { [array anymore game_points $search] } { } { 
    set key [array nextelement game_points $search] 

    if {$game_points($key) > $winpoints1} {
      set win3 $win2
      set win2 $win1

      set winpoints3 $winpoints2
      set winpoints2 $winpoints1

      set winpoints1 $game_points($key)
      set win1 $key  

    } elseif {$game_points($key) > $winpoints2} {
      set win3 $win2
      set winpoints3 $winpoints2

      set winpoints2 $game_points($key)
      set win2 $key

    } elseif {$game_points($key) > $winpoints3} {
      set winpoints3 $game_points($key)
      set win3 $key
    }

  }

  array donesearch game_points $search

  set output "PRIVMSG $chan :Cryptoquote won by $win1 with $winpoints1 points; 3 points added to overall score "
  
  # Copy channel_playerscores to local namespace if it exists
  if {[info exists l_chan_player_scores($chan)]} {
    array set player_scores $l_chan_player_scores($chan)
  } 

  if {[info exists player_scores($win1)]} {
    set player_scores($win1) [expr { $player_scores($win1) + 3 }]
  } else {
    set player_scores($win1) 3
  }

  append output "($player_scores($win1))"


  if {[string length $win2] > 0} {
    append output ", $win2 came in 2nd with $winpoints2 points; 2 points added to overall score "

    if {[info exists player_scores($win2)]} {
      set player_scores($win2) [expr { $player_scores($win2) + 2 }]
    } else {
      set player_scores($win2) 2
    }

    append output "($player_scores($win2))"

  }

  if {[string length $win3] > 0} {
    append output ", $win3 came in 3rd with $winpoints3; 1 point added to overall score "

    if {[info exists player_scores($win3)]} {
      incr player_scores($win3)
    } else {
      set player_scores($win3) 1
    }

    append output "($player_scores($win3))"

  }

  set l_chan_player_scores($chan) [array get player_scores]

  putserv $output

  cryptoquote:write_scores

  #cryptoquote:show_scores "" "" "" $chan ""

  set a_chan_in_game($chan) 0

}

proc cryptoquote:read_quotes { } {
  global l_quotes crypto_quotes_file

  set fd [open $crypto_quotes_file "r"]
  set l_quotes [split [read $fd] "\n"]
  close $fd
}

cryptoquote:read_quotes
cryptoquote:read_scores


puts "Loaded cryptoquote"
