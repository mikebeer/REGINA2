/*
 * This program converts a generic Win32 resource file into a version-specific
 * one.
 * Parameters:
 *   Input:   input rc file
 *   Output:  output rc file with version numbers
 *   Version: in x.x or x.x.x format
 *   Architecture (x86/x64/arm64)
 *   Interpreter: Regina, ooRexx, etc
 *   Version Date: DD MON YYYY
 *   Variant: Optional: a string
 * Assumptions: input file exists and in correct format
 *              output file is writeable
 */
Trace O
Parse Arg input output version arch interpreter verdd vermm veryy variant
If veryy = '' Then
   Do
      Say 'Invalid value for version date.'
      Exit 1
   End
verdd = Strip( verdd, 'B', '"' )
vermm = Strip( vermm, 'B', '"' )
veryy = Strip( veryy, 'B', '"' )
verdate = verdd vermm veryy
interpreter = Strip( interpreter, 'B', '"' )
verdot = version
num_dots = Countstr( '.', verdot )
Select
   When num_dots = 1 Then
      Do
         qm_ver = '?.?'
         qm_vercomma = '?,?'
      End
   When num_dots = 2 Then
      Do
         qm_ver = '?.?.?'
         qm_vercomma = '?,?,?'
      End
   Otherwise Say 'Invalid verdot format:' verdot'. Must be x.x or x.x.x'
End
vercomma = Changestr( '.', version, ',' )
veryear = Left( Date( 'S' ), 4 )
Call Stream input, 'C', 'OPEN READ'
Do While Lines( input) > 0
   line = Linein( input )
   If num_dots = 2 & Countstr( '?.?.0', line ) = 1 Then line = Changestr( '?.?.0', line, verdot )
   If num_dots = 2 & Countstr( '?,?,0', line ) = 1 Then line = Changestr( '?,?,0', line, vercomma )
   line = Changestr( qm_ver, line, verdot )
   line = Changestr( qm_vercomma, line, vercomma )
   line = Changestr( '????????', line, verdate )
   line = Changestr( '????', line, veryear )
   line = Changestr( '??bits??', line, arch )
   line = Changestr( '??interpreter??', line, interpreter )
   line = Changestr( '??variant??', line, variant )
   Queue line
End
Call Stream input, 'C', 'CLOSE'
-- input can be same as output, so need to close input first
Call Stream output, 'C', 'OPEN WRITE REPLACE'
Do Queued()
   Parse Pull line
   Call Lineout output, line
End
Call Stream output, 'C', 'CLOSE'
Return 0
