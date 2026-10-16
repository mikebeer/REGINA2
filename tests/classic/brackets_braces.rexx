/* square brackets/braces are only legal inside strings and comments */
say "[1]" '{x}' /* [comment] {more} */
s = 'a[1]'; say s length(s)
parse value '[x]' with '[' v ']'; say v
say translate('[ab]', '()', '[]')
