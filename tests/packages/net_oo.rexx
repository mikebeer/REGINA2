/* OOREGINA - rgnet, an RxSock-style test package, with OO features */
parse arg host port
call rxfuncadd 'NetLoadFuncs', 'rgnet', 'NetLoadFuncs'
call NetLoadFuncs
say 'host:' NetHostByName('localhost', 'host.') host.addr
s = NetSocket()
addr.port = port; addr.addr = host.addr
say 'connect:' NetConnect(s, 'addr.')
msgs = .array~of('hello', 'from', 'Regina OO')
say 'sent:' NetSend(s, msgs~inject('', { :a :x | strip(a x) }))
do i = 1 to 2000; junk = .array~of(i, .directory~new); end   /* garbage */
say 'received:' NetRecv(s, 'answer', 1000) answer
call NetClose s
words = .bag~new; do w over answer~makearray(' '); words~add(w); end
say 'words:' words~items
call NetDropFuncs
