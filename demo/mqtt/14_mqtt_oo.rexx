/* 14_mqtt_oo.rexx - MQTT with the Mqtt class
 *
 * A calculator service that answers requests sent over MQTT, written
 * with ::requires 'mqtt'. The client and the service use separate
 * connections; listen calls a block for every message.
 *
 *   regina 14_mqtt_oo.rexx [host [port]]       (default localhost 1883)
 */
parse arg host port
if host = '' then host = 'localhost'
if port = '' then port = 1883

service = .mqtt~connect(host, port, 'calc-service')
if service == .nil then do
  say 'no connection to' host':'port '-' .mqtt~error
  exit 1
end
client = .mqtt~connect(host, port, 'calc-client')
say 'connected to' service~host

service~subscribe('calc/request', 1)
client~subscribe('calc/reply/+', 1)

/* the client sends its requests: id, operand, operator, operand */
jobs = .array~of('1 6 * 7', '2 2 ** 10', '3 22 / 7', '4 1 / 0', '5 17 // 5')
do job over jobs
  client~publish('calc/request', job, 1)
end

/* the service answers each request */
calc = .Calculator~new(service)
say 'requests answered:' service~listen({ use arg m; calc~answer(m) }, 1)

/* the client collects the replies, sorted by request id */
replies = .sortedCollection~new
client~listen({ use arg m; replies~add(m~topic~substr(12)':' m~message) }, 1)
do r over replies
  say r
end

/* listen stops early when the block returns 0 */
do i = 1 to 3; client~publish('calc/reply/x', 'note' i); end
say 'notes read:' client~listen({ :m | m~message \== 'note 2' }, 1) 'left:' client~pending
say 'last:' client~receive(1)

client~disconnect
service~disconnect
exit 0

::requires 'mqtt'

::class Calculator
::method init
  expose mqtt
  use arg mqtt

::method answer                         /* m: an MqttMessage */
  expose mqtt
  use arg m
  parse value m~message with id a op b
  select
    when \datatype(a, 'N') | \datatype(b, 'N') then r = 'not a number'
    when op = '+'  then r = a + b
    when op = '-'  then r = a - b
    when op = '*'  then r = a * b
    when op = '**' then r = a ** b
    when op = '//' then r = a // b
    when op = '/' & b = 0 then r = 'division by zero'
    when op = '/'  then r = format(a / b, , 4)
    otherwise r = 'unknown operator' op
  end
  mqtt~publish('calc/reply/'id, a op b '=' r, 1)
