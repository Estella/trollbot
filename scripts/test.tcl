bind pub - "!hi" do:hi
bind pub - "!test" do:test
bind pub - "!countusers" do:countusers

proc do:test { nick uhost hand chan arg } {
  putserv "PRIVMSG $chan :Result: [matchwild [lindex $arg 0] [lindex $arg 1]]"
}


proc do:hi { nick uhost hand chan arg } {
  putserv "PRIVMSG $chan :hi \"$nick\" \"$uhost\" \"$hand\" \"$chan\" \"$arg\""
}

proc do:countusers { nick uhost hand chan arg } {
  putserv "PRIVMSG $chan :[countusers]"
}

puts "test loaded"
