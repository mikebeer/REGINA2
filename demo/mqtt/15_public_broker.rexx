/* 15_public_broker.rexx - MQTT with a public test broker
 *
 * Uses test.mosquitto.org, the free test server of the Eclipse Mosquitto
 * project, so it runs without a broker of your own:
 *
 *   regina 15_public_broker.rexx                         port 1883, anonymous
 *   regina 15_public_broker.rexx test.mosquitto.org 1884 rw readwrite
 *   regina 15_public_broker.rexx myhost 1883             any other broker
 *
 * RgMqtt has no TLS, so use the ports without encryption (1883, 1884).
 * Everybody can read what is published on a public broker: never send
 * anything private. The program works below a topic of its own,
 * regina-oo/<client id>, and removes its retained message at the end.
 */
parse arg host port user password
if host = '' then host = 'test.mosquitto.org'
if port = '' then port = 1883

/* a client id nobody else uses */
id = 'regina-oo-' || date('B') || right(time('S'), 5, '0') || random(1000, 9999)
root = 'regina-oo/'id

opts = .directory~new
opts~keepAlive = 30
opts~timeout = 20                      /* public servers can be slow */
opts~willTopic = root'/status'; opts~willMessage = 'gone'
if user \= '' then do; opts~user = user; opts~password = password; end
say 'connecting to' host 'port' port
c = .mqtt~connect(host, port, id, opts)
if c == .nil then do
  say 'no connection:' .mqtt~error
  exit 1
end

/* the broker's version, if it publishes it in $SYS */
if c~subscribe('$SYS/broker/version') >= 0 then do
  m = c~receive(3)
  if m \== .nil then say 'broker:' m~message
  c~unsubscribe('$SYS/broker/version')
end

/* publish to our own topics and read the messages back */
say 'subscribe:' c~subscribe(root'/#', 2)
c~publish(root'/status', 'online', 1, 1)            /* retained */
do qos = 0 to 2
  c~publish(root'/greeting/qos'qos, 'Hello from ooRegina, QoS' qos, qos)
end
got = .sortedCollection~new
n = c~listen({ use arg m; got~add(m~topic~substr(length(root) + 2) '=' m~message) }, 5)
say n 'messages came back:'
do line over got; say ' ' line; end
say 'ping:' c~ping

/* remove the retained message and leave */
c~publish(root'/status', '', 1, 1)
say 'disconnect:' c~disconnect
exit 0
::requires 'mqtt'
