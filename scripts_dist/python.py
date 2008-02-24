##
## python evaluation module
##
import sys

saved = {}

def _parse_code(code):
   #; represents a new line, always
   lines = code.split("#end")#so we don't need a ; after #end
   code = "#end;".join(lines) 
   lines = code.split(";") #eventually we need a regex to detect if the ; is in a string
   #now, we build formatting.  Any line that ends with a : means a new indent level
   #and that indent level remains until we get a #end tag
   indent = 0
   nlines = []
   for line in lines:
      line = line.replace("import", "zzzz")
      line = line.replace("exec", "zzzz")
      line = line.replace("getattr", "zzzz")
      line = line.replace("setattr", "zzzz")
      line = line.strip()
      if line[0:4] == "#end":
         indent -= 1
      space = '  ' * indent
      nlines.append(space + line)
      
      if line[-1] == ":":
         #new indent level
         indent += 1
      #end if
   #end for
   return nlines

def _buildcode(func, lines):
   code = "def %s(irc):\n" % (func)
   code += "  " + "\n  ".join(lines)
   return code+"\n"

def run_python(irc):
   #execute python code
   trollbot.log(trollbot.LOG_DEBUG, "Got to python callback")
   codestr = str(irc['text']) #we want the raw code string; no fancy parsing
   lines = _parse_code(codestr)
   code = _buildcode("_tmp",lines)
   trollbot.log(trollbot.LOG_DEBUG, "CODE:\n%s" % (code))
   try:
      exec(code)
      trollbot.privmsg(irc['channel'], str(_tmp(irc)))

   except SyntaxError, se:
      trollbot.log(trollbot.LOG_DEBUG, str(se))
      parts = dir(se)
      for part in parts:
         if parts[0] == "_":
            continue
         trollbot.log(trollbot.LOG_DEBUG, "%s = %s" % (part, getattr(se, part)))
      trollbot.log(trollbot.LOG_DEBUG, "LINE NO: %s" % (se.lineno-2))
      trollbot.log(trollbot.LOG_DEBUG, ";".join(lines))
      line = lines[se.lineno-2]
      trollbot.privmsg(irc['channel'], "Syntax Error: %s" % (str(se)))
      trollbot.privmsg(irc['channel'], " >> %s" % (line))
      return

def load():
   trollbot.bind(trollbot.TRIG_PUB, "-", "!py", "python.run_python")
