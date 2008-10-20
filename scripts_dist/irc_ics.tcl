bind pub - "!score" do_ics_score


proc do_ics_score { nick uhost hand chan arg } {
	set score [ics_get_score "freechess.org" $arg]

	if {$score != 0} {
		puthelp "PRIVMSG $chan :The score for $arg is $score"
	}
}
