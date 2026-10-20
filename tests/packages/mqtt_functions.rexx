/* RgMqtt: the function interface, from a classic program */
parse arg host port
call RxFuncAdd 'MqttLoadFuncs', 'rgmqtt', 'MqttLoadFuncs'
call MqttLoadFuncs
say MqttVersion()

h = MqttConnect('127.0.0.1', port, 'functions')
say 'connect:' h
say 'subscribe:' MqttSubscribe(h, 'test/#', 2) MqttSubscribe(h, 'other/+/x', 1)
do q = 0 to 2
  say 'publish qos' q':' MqttPublish(h, 'test/q'q, 'message' q, q)
end
say 'publish binary:' MqttPublish(h, 'other/a/x', '00FF0D0A'x, 1)
big = copies('0123456789', 20000)
say 'publish 200 KB:' MqttPublish(h, 'test/big', big, 1)
say 'not matched:' MqttPublish(h, 'other/a/y', 'no')
say 'pending:' MqttPending(h)
do while MqttReceive(h, 'm.', 1)
  if length(m.message) > 50 then m.message = length(m.message) 'bytes, same:' (m.message == big)
  text = ''
  if verify(m.message, xrange(' ', '~')) = 0 then text = '('m.message')'
  say ' ' m.topic m.qos m.retain c2x(m.message) text
end
say 'ping:' MqttPing(h)

/* retained messages */
say 'retain:' MqttPublish(h, 'keep/me', 'kept value', 1, 1)
h2 = MqttConnect('127.0.0.1', port, 'second')
say 'second:' h2 MqttSubscribe(h2, 'keep/#', 1) MqttReceive(h2, 'r.', 2) r.topic r.message r.retain

/* a will: h3 is closed without DISCONNECT */
w.willtopic = 'test/will'; w.willmessage = 'h3 went away'; w.willqos = 1
h3 = MqttConnect('127.0.0.1', port, 'third', 'w.')
call MqttPublish h3, '$broker/kick', ''
say 'will:' MqttReceive(h, 'm.', 2) m.topic '-' m.message
say 'after kick:' MqttPing(h3) MqttError()

say 'unsubscribe:' MqttUnsubscribe(h, 'test/#')
call MqttPublish h, 'test/q0', 'unseen'
say 'nothing:' MqttReceive(h, 'm.', 0.3)
say 'denied:' MqttSubscribe(h, '$denied/x')

/* errors */
say 'wildcard topic:' MqttPublish(h, 'test/#', 'x') MqttError()
say 'bad handle:' MqttPublish(99, 'a', 'b') MqttError()
u.user = 'bad'; u.password = 'secret'
say 'refused:' MqttConnect('127.0.0.1', port, 'x', 'u.') MqttError()
say 'no host:' MqttConnect('no.such.host.invalid') MqttError()
signal on syntax
say MqttPublish(h, 'a', 'b', 3)
back:
say 'disconnect:' MqttDisconnect(h) MqttDisconnect(h2) MqttDisconnect(h)
call MqttDropFuncs
say 'dropped:' rxfuncquery('MqttConnect')
exit
syntax:
say 'qos 3: error' rc
signal back
