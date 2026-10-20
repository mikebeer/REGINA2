/* OOREGINA - RgMqtt through the Mqtt class of ::requires 'mqtt' */
parse arg host port
say .mqtt~version
c = .mqtt~connect('127.0.0.1', port, 'oo-client', .directory~new~~put(10, 'keepAlive'))
if c == .nil then do; say 'failed:' .mqtt~error; exit 1; end
say 'connected to' c~host
say 'subscribe:' c~subscribe('sensor/#', 1)
temps = .directory~new
temps~kitchen = 21.5; temps~garden = 12; temps~cellar = 9.5
do room over temps~allIndexes~sortWith({ :a :b | a <= b })
  c~publish('sensor/'lower(room), temps[room], 1)
end
seen = .table~new
n = c~listen({ use arg m; seen[m~topic] = m~message; say ' ' m }, 1)
say 'listened:' n 'average:' seen~inject(0, { :s :x | s + x }) / seen~items
c~publish('sensor/stop', 'now')
c~publish('sensor/later', 'x')
say 'until stop:' c~listen({ :m | m~topic \== 'sensor/stop' }, 1) 'pending:' c~pending
m = c~receive(1); say 'left:' m~topic m~qos m~retain m~dup
say 'none:' c~receive(0.2)
say 'refused:' .mqtt~connect('127.0.0.1', port, 'x', .directory~new~~put('bad', 'user')) .mqtt~error
say 'disconnect:' c~disconnect c~isConnected
::requires 'mqtt'
