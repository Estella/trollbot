ics_bind notify  - "*tehcheckersking*" follow_tehcheckersking
ics_bind notify  - "*saldeeznuts*"     follow_sal
ics_bind connect - "*"                 initiate_stalking
ics_bind game    - "*"                 announce_game
ics_bind move    - "*"                 announce_move
ics_bind endgame - "*"                 announce_endgame

# changes are flag changes
# flags are as follows:
# 
#  * pawn_double_push_file
#  * white_can_castle_short
#  * black_can_castle_short
#  * white_can_castle_long
#  * black_can_castle_long
#  * last_irreversible_move
# 
# board is a list with lists by rank
# [lindex $board 0] = 1st rank

proc announce_endgame { ics game_id white black winner loser result end_message } {
	if {$result == "checkmate"} {
		irc_interp "DALnet" "puthelp \"PRIVMSG #php :(Game:$game_id) has ended. $winner delivered an ass kicking to $loser.\""
	}

	if {$result == "stalemate"} {
		irc_interp "DALnet" "puthelp \"PRIVMSG #php :(Game:$game_id) has ended in stalemate.\""
	}

	if {$result == "draw"} {
		irc_interp "DALnet" "puthelp \"PRIVMSG #php :(Game:$game_id) has ended. Both sides thought they sucked, so they called a draw.\""
	}
}

proc announce_move { ics game_id move_id } {
	
}

proc announce_game { ics game_id white black start_time time_increment } {
	irc_interp "DALnet" "puthelp \"PRIVMSG #php :(Game:$game_id) New FICS game started. White is played by $white and black by $black.\""
}


proc initiate_stalking { ics who } {
	putics $ics "+notify saldeeznuts"
	putics $ics "+notify tehcheckersking"
	putics $ics "follow tehcheckersking"
	putics $ics "follow saldeeznuts"
}

proc follow_tehcheckersking { ics who action } {
	if {$action == "arrived."} {
		irc_interp "DALnet" "puthelp \"PRIVMSG #php :tehcheckersking (poutine) has been spotted on freechess. Now following his games.\""
		putics $ics "follow tehcheckersking"
	} elseif {$action == "departed."} {
		irc_interp "DALnet" "puthelp \"PRIVMSG #php :tehcheckersking (poutine) has left freechess.\""
	}

	return
}

proc follow_sal { ics who action } {
	if {$action == "arrived."} {
		irc_interp "DALnet" "puthelp \"PRIVMSG #christian_debate :Saldeeznuts (sal_dz) has been spotted on freechess. Starting to stalk that greasy bastard.\""
		putics $ics "follow saldeeznuts"
	} elseif {$action == "departed."} {
		irc_interp "DALnet" "puthelp \"PRIVMSG #php :saldeeznuts (sal_dz) has left freechess.\""
	}

}

puts "FICS Notify Stalker Loaded."
