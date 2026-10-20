/* rgnet, an RxSock-style test package, used by a classic program */
parse arg host port
call rxfuncadd 'NetLoadFuncs', 'rgnet', 'NetLoadFuncs'
call NetLoadFuncs
say 'host:' NetHostByName('localhost', 'host.') host.addr
s = NetSocket()
addr.port = port; addr.addr = host.addr
say 'connect:' NetConnect(s, 'addr.')
say 'sent:' NetSend(s, 'hello from classic Regina')
say 'received:' NetRecv(s, 'answer', 1000) answer
call NetClose s
call NetDropFuncs
say 'dropped:' rxfuncquery('NetSend')
