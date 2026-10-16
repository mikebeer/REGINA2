#!/usr/bin/env regina
/*
 * This is a program to try Rexx code. Regina specific as it uses the regutil
 * functions sysgeline() and sysgetlinehistory() for manipulation of command
 * history.
 */
Call RxFuncAdd 'sysloadfuncs', 'regutil', 'sysloadfuncs'
Call sysloadfuncs
Signal On Syntax
Parse Source src
Say 'rexxtry.rexx for' src
Say
Say 'Enter Rexx code at the REXXTRY=> prompt and hit Enter to execute.'
Say 'Enter "exit" at the prompt and hit Enter to exit the program.'
Say 'Use arrow keys to retrieve previously entered code or to modify a retrieved line.'
Say
histfile = 'rexxtry.history'
rcode= sysgetlinehistory( histfile, 'INIT' )
rcode= sysgetlinehistory( histfile, 'READ' )
loop:
Signal On Syntax
Do Forever
   line = sysgetline('REXXTRY=> ')
   If Translate( line ) = 'EXIT' Then Leave
   Interpret line
End
rcode= sysgetlinehistory( histfile, 'WRITE' )
Return 0

Syntax:
Say 'Syntax error:'
Say Condition( 'd')
Signal loop
Return 0
