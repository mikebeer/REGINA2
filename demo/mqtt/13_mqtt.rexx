/* 13_mqtt.rexx - MQTT with the RgMqtt function package
 *
 * A small home monitor: one connection publishes temperatures, another
 * one subscribes, computes statistics and raises an alarm. A third
 * connection, opened later, still gets the last values because they are
 * retained by the broker. Runs with any broker:
 *
 *   regina 13_mqtt.rexx [host [port]]          (default localhost 1883)
 */
parse arg host port
if host = '' then host = 'localhost'
if port = '' then port = 1883
call RxFuncAdd 'MqttLoadFuncs', 'rgmqtt', 'MqttLoadFuncs'
call MqttLoadFuncs

/* the monitor: its will tells others when it disappears */
opt.willtopic = 'home/status/monitor'; opt.willmessage = 'offline'
monitor = MqttConnect(host, port, 'home-monitor', 'opt.')
if monitor = 0 then do
  say 'no connection to' host':'port '-' MqttError()
  exit 1
end
call MqttSubscribe monitor, 'home/+/temperature', 1
call MqttSubscribe monitor, 'home/alarm', 1

/* the sensors publish retained readings with QoS 1 */
sensors = MqttConnect(host, port, 'home-sensors')
reading.1 = 'kitchen 21.5'; reading.2 = 'garden 12.0'
reading.3 = 'cellar 9.5';   reading.4 = 'office 26.5'
reading.0 = 4
do i = 1 to reading.0
  parse var reading.i room value
  call MqttPublish sensors, 'home/'room'/temperature', value, 1, 1
end

/* the monitor collects the readings and raises an alarm above 25 */
count = 0; sum = 0; low = ''; high = ''
do while MqttReceive(monitor, 'msg.', 1)
  if msg.topic = 'home/alarm' then do
    say 'ALARM received:' msg.message
    iterate
  end
  parse var msg.topic 'home/' room '/temperature'
  say left(room, 8) right(msg.message, 5) 'C'
  count = count + 1; sum = sum + msg.message
  if low = '' | msg.message < low then low = msg.message
  if high = '' | msg.message > high then high = msg.message
  if msg.message > 25 then
    call MqttPublish monitor, 'home/alarm', room 'is too warm:' msg.message 'C', 1
end
say count 'readings, average' format(sum / count, , 1) 'C, from' low 'to' high

/* a late subscriber gets the retained values */
late = MqttConnect(host, port, 'home-late')
call MqttSubscribe late, 'home/+/temperature', 1
n = 0
do while MqttReceive(late, 'msg.', 1)
  n = n + 1; got.n = msg.topic msg.message msg.retain
end
call sort
say 'a late subscriber got' n 'retained values:'
do i = 1 to n
  parse var got.i topic value retained
  say ' ' topic '=' value '(retained:' retained')'
end

/* remove the retained values again and disconnect */
do i = 1 to reading.0
  parse var reading.i room .
  call MqttPublish sensors, 'home/'room'/temperature', '', 1, 1
end
call MqttDisconnect late
call MqttDisconnect sensors
call MqttDisconnect monitor
exit 0

sort: procedure expose got. n          /* insertion sort of got.1 .. got.n */
  do i = 2 to n
    x = got.i
    do j = i - 1 to 1 by -1 while got.j >> x
      k = j + 1; got.k = got.j
    end
    k = j + 1; got.k = x
  end
  return
