bind pub - "!hi" do:hi
bind pub - "!test" do:test
bind pub - "!countusers" do:countusers
bind pub - "!matchattr" do:matchattr
bind pub - "!onchan" do:onchan
bind pub - "!botname" do:botname

proc do:botname { nick uhost hand chan arg } {
	putserv "PRIVMSG $chan :Result: [botname]";
}

proc do:onchan { nick uhost hand chan arg } {
	putserv "PRIVMSG $chan :Result: [onchan [lindex $arg 0] [lindex $arg 1]]"
}

proc do:test { nick uhost hand chan arg } {
  putserv "PRIVMSG $chan :Result: [matchwild [lindex $arg 0] [lindex $arg 1]]"
}


proc do:hi { nick uhost hand chan arg } {
  putserv "PRIVMSG $chan :hi \"$nick\" \"$uhost\" \"$hand\" \"$chan\" \"$arg\""
}

proc do:countusers { nick uhost hand chan arg } {
  putserv "PRIVMSG $chan :[countusers]"
}

proc do:matchattr { nick uhost hand chan arg } {
  if {[matchattr [lindex $arg 0] [lindex $arg 1]] == 1} {
    putserv "PRIVMSG $chan :Found flag"
  } else {
    putserv "PRIVMSG $chan :Did not find flag"
  }
}

puts "test loaded"
