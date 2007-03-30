bind pub - "!hi" do:hi
bind pubm - "*test*" do:test

proc do:test { nick uhost hand chan text } {
  putserv "PRIVMSG $chan :Got triggered"
}

proc do:hi { nick uhost hand chan arg } {
  putserv "PRIVMSG $chan :hi \"$nick\" \"$uhost\" \"$hand\" \"$chan\" \"$arg\""
}


puts "test loaded"
