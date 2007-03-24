bind pub - "!hi" do:hi

proc do:hi { nick uhost hand chan arg } {
  putserv "PRIVMSG $chan :hi \"$nick\" \"$uhost\" \"$hand\" \"$chan\" \"$arg\""
}


puts "test loaded"
