/* OOREGINA */
/*
 * The workspace session of Regina OO, started with "rexx --ws [wsid]".
 * Lines are Rexx clauses; an expression on its own shows its value, as in
 * APL (and the environment WS shows commands). Lines that begin with "::"
 * define routines and classes up to an empty line ([]FX). System commands
 * begin with ")": )SAVE )LOAD )OUT )IN )VARS )FNS )CLASSES )ERASE )WSID
 * )CLEAR )SHOW )GC )HELP )OFF. All variables of the session are named
 * __WS.something, which []NL and []SAVE leave out.
 */
parse arg __ws.arg
say 'Regina OO workspace session. )HELP lists the commands, )OFF ends it.'
address WS
if __ws.arg \= '' then do
   signal on syntax name __wsStartErr
   call __wsCommand ')LOAD' __ws.arg
end
__wsLoop:
__ws.eof = 0
call on notready name __wsEOF
signal on syntax name __wsErr
signal on halt name __wsHalt
do forever
   call charout , '      '
   __ws.line = linein()
   if __ws.eof = 1 then
      exit 0
   if strip(__ws.line) == '' then
      iterate
   if left(strip(__ws.line), 1) == ')' then do
      call __wsCommand strip(__ws.line)
      iterate
   end
   if left(strip(__ws.line), 2) == '::' then do
      /* a definition: up to an empty line */
      __ws.def = __ws.line
      do forever
         call charout , '    : '
         __ws.more = linein()
         if strip(__ws.more) == '' | __ws.eof = 1 then
            leave
         __ws.def = __ws.def || '0a'x || __ws.more
      end
      say []FX(__ws.def)
      iterate
   end
   if __wsIsInstruction(__ws.line) then
      interpret __ws.line
   else do
      /* an expression: show its value; a message without result runs */
      __ws.trying = 1
      interpret '__ws.value =' __ws.line
      __ws.trying = 0
      say __ws.value
      drop __ws.value
   end
end

__wsErr:
   if __ws.trying = 1 & rc = 91 then do
      /* "obj~method" that returns nothing: an instruction after all */
      __ws.trying = 0
      signal on syntax name __wsErr
      interpret __ws.line
      signal __wsLoop
   end
   __ws.trying = 0
   if condition('D') == '' then
      say 'Error' rc':' errortext(rc)
   else
      say condition('D')
   signal __wsLoop

__wsStartErr:
   say condition('D')
   signal __wsLoop

__wsEOF:   /* the end of the input ends the session */
   __ws.eof = 1
   return

__wsHalt:
   say 'INTERRUPT'
   signal __wsLoop

__wsCommand:   /* no PROCEDURE: the commands work on the session's variables */
   parse arg ')' __ws.cmd __ws.rest
   __ws.cmd = translate(__ws.cmd)
   __ws.rest = strip(__ws.rest)
   select
      when abbrev('SAVE', __ws.cmd, 2) then do
         if __ws.rest == '' then
            say 'SAVED' []SAVE()
         else
            say 'SAVED' []SAVE(__ws.rest)
      end
      when abbrev('LOAD', __ws.cmd, 2) then do
         call []LOAD __ws.rest
         say 'LOADED' []WSID()
      end
      when abbrev('OUT', __ws.cmd, 2) then do
         parse var __ws.rest __ws.file __ws.names
         if __ws.names == '' then
            say 'WRITTEN' []OUT(__ws.file)
         else
            say 'WRITTEN' []OUT(__ws.file, __ws.names)
      end
      when abbrev('IN', __ws.cmd, 2) then do
         parse var __ws.rest __ws.file __ws.names
         if __ws.names == '' then
            say []IN(__ws.file)
         else
            say []IN(__ws.file, __ws.names)
      end
      when abbrev('VARS', __ws.cmd, 1) then say []NL(2)
      when abbrev('FNS', __ws.cmd, 1) then say []NL(3)
      when abbrev('CLASSES', __ws.cmd, 2) then say []NL(9)
      when abbrev('ERASE', __ws.cmd, 1) then call []EX __ws.rest
      when abbrev('WSID', __ws.cmd, 1) then do
         if __ws.rest == '' then
            say []WSID()
         else
            say 'WAS' []WSID(__ws.rest)
      end
      when abbrev('CLEAR', __ws.cmd, 2) then do
         call []CLEAR
         say 'CLEAR WS'
      end
      when abbrev('SHOW', __ws.cmd, 2) then say []CR(__ws.rest)
      when abbrev('GC', __ws.cmd, 1) then say []GC() 'objects freed'
      when abbrev('OFF', __ws.cmd, 1) | abbrev('CONTINUE', __ws.cmd, 4) then exit 0
      when abbrev('HELP', __ws.cmd, 1) then do
         say ')SAVE [wsid]          save the workspace in wsid.rxws'
         say ')LOAD wsid            clear, then load wsid.rxws'
         say ')OUT file [names]     write all or some names to file.rxo'
         say ')IN file [names]      read all or some names from a file'
         say ')VARS )FNS )CLASSES   list variables, routines, classes'
         say ')ERASE names          erase variables, routines, classes'
         say ')SHOW name            show the source of a routine or class'
         say ')WSID [name]          show or set the workspace name'
         say ')CLEAR                empty workspace'
         say ')GC                   collect unused objects now'
         say ')OFF                  end the session'
         say 'A line with an expression shows its value. Lines that start'
         say 'with :: define routines and classes, up to an empty line.'
      end
      otherwise say 'INCORRECT COMMAND'
   end
   return

/* 1 if the line is an instruction or an assignment, 0 for an expression */
__wsIsInstruction:
   parse arg __ws.l
   parse upper var __ws.l __ws.w1 .
   if wordpos(__ws.w1, 'ADDRESS ARG CALL DO DROP EXIT IF INTERPRET ITERATE LEAVE',
      'NOP NUMERIC OPTIONS PARSE PROCEDURE PULL PUSH QUEUE RETURN SAY SELECT',
      'SIGNAL TRACE UPPER USE EXPOSE LOOP END WHEN OTHERWISE THEN ELSE') > 0 then
      return 1
   /* several clauses; a label */
   __ws.q = ''
   __ws.depth = 0
   do __ws.i = 1 to length(__ws.l)
      __ws.c = substr(__ws.l, __ws.i, 1)
      if __ws.q \== '' then do
         if __ws.c == __ws.q then __ws.q = ''
         iterate
      end
      select
         when __ws.c == "'" | __ws.c == '"' then __ws.q = __ws.c
         when __ws.c == '(' | __ws.c == '[' | __ws.c == '{' then __ws.depth = __ws.depth + 1
         when __ws.c == ')' | __ws.c == ']' | __ws.c == '}' then __ws.depth = __ws.depth - 1
         when __ws.depth > 0 then nop
         when __ws.c == ';' then return 1
         when __ws.c == ':' & __ws.i > 1 & substr(__ws.l, __ws.i + 1, 1) \== ':' then
            if verify(strip(left(__ws.l, __ws.i - 1)), ,
               'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_.!?') = 0 then
               return 1
         when __ws.c == '=' then do
            /* "target = value": the target has no blanks outside brackets */
            __ws.prev = substr(__ws.l, max(__ws.i - 1, 1), 1)
            if __ws.i = 1 | pos(__ws.prev, '\<>=') > 0 then return 0
            if substr(__ws.l, __ws.i + 1, 1) == '=' then return 0
            return __wsNoBlanks(strip(left(__ws.l, __ws.i - 1)))
         end
         otherwise nop
      end
   end
   return 0

__wsNoBlanks:
   parse arg __ws.t
   __ws.d = 0
   __ws.q = ''
   do __ws.k = 1 to length(__ws.t)
      __ws.c = substr(__ws.t, __ws.k, 1)
      if __ws.q \== '' then do
         if __ws.c == __ws.q then __ws.q = ''
         iterate
      end
      if __ws.c == "'" | __ws.c == '"' then __ws.q = __ws.c
      else if pos(__ws.c, '([{') > 0 then __ws.d = __ws.d + 1
      else if pos(__ws.c, ')]}') > 0 then __ws.d = __ws.d - 1
      else if __ws.c == ' ' & __ws.d = 0 then return 0
   end
   return __ws.t \== ''
