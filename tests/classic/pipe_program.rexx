/* a program read from a pipe: several lines, a label, CR LF line ends */
say 'first line'
call sub 'x'
say 'after the call' result
exit
sub: return 'sub got' arg(1)
