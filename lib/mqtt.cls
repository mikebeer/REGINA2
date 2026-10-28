/* mqtt.cls - MQTT classes for ooRegina
 *
 * ::requires 'mqtt' loads this library. It is built into the interpreter
 * and uses the RgMqtt function package (librgmqtt.so, rgmqtt.dll), which
 * must be installed. A file named mqtt.cls next to the program or in
 * REGINA_MACROS takes precedence.
 *
 *   c = .mqtt~connect('localhost')               -- .nil if that failed
 *   if c == .nil then say .mqtt~error
 *   c~subscribe('sensors/#')
 *   c~publish('sensors/kitchen', 21.5)
 *   c~listen({ :m | say m~topic '=' m~message }, 10)
 *   c~disconnect
 */
::class Mqtt public

::method loadFunctions class private
  if rxfuncquery('MqttConnect') then do
    if rxfuncadd('MqttLoadFuncs', 'rgmqtt', 'MqttLoadFuncs') \= 0 then return 0
    if rxfuncquery('MqttLoadFuncs') then return 0
    call MqttLoadFuncs
  end
  return 1

/* .mqtt~connect(host [, port] [, clientId] [, options]) -> an Mqtt or .nil
   options: a Directory with keepAlive, clean, user, password, willTopic,
   willMessage, willQos, willRetain, timeout */
::method connect class
  use arg host, port, clientid, options
  if arg(2, 'o') then port = 1883
  if arg(3, 'o') then clientid = ''
  if arg(4, 'o') then options = .nil
  if \self~loadFunctions then do
    self~setError('the RgMqtt package (rgmqtt) is not installed')
    return .nil
  end
  drop opt.
  if options \== .nil then do
    s = options~supplier
    do while s~available
      k = translate(s~index)
      opt.k = s~item
      s~next
    end
  end
  h = MqttConnect(host, port, clientid, 'opt.')
  if h = 0 then do
    self~setError(MqttError())
    return .nil
  end
  self~setError('')
  return self~new(h, host, port)

::method error class
  expose lastError
  if symbol('lastError') == 'VAR' then return lastError
  return ''

::method setError class
  expose lastError
  use arg lastError

::method version class
  if \self~loadFunctions then return ''
  return MqttVersion()

::attribute handle get
::attribute host get
::attribute port get

::method init
  expose handle host port
  use arg handle, host, port

/* publish(topic, message [, qos] [, retain]) -> 1, or 0 if it failed */
::method publish
  expose handle
  use arg topic, message, qos, retain
  if arg(2, 'o') then message = ''
  if arg(3, 'o') then qos = 0
  if arg(4, 'o') then retain = 0
  return MqttPublish(handle, topic, message, qos, retain) = 0

/* subscribe(filter [, qos]) -> the QoS the broker granted, -1 if refused */
::method subscribe
  expose handle
  use arg filter, qos
  if arg(2, 'o') then qos = 0
  r = MqttSubscribe(handle, filter, qos)
  if r = 128 then return -1
  return r

::method unsubscribe
  expose handle
  use arg filter
  return MqttUnsubscribe(handle, filter) = 0

/* receive([seconds]) -> an MqttMessage, or .nil if none came in time */
::method receive
  expose handle
  use arg seconds
  if arg(1, 'o') then seconds = 0
  if MqttReceive(handle, 'm.', seconds) \= 1 then return .nil
  return .MqttMessage~new(m.topic, m.message, m.qos, m.retain, m.dup)

/* listen(block [, seconds]): calls block with each message until no
   message comes for seconds (forever if omitted) or the block returns 0.
   Returns the number of messages handled. */
::method listen
  use arg block, seconds
  if arg(2, 'o') then seconds = ''
  n = 0
  do forever
    if seconds == '' then do
      m = self~receive(3600)
      if m == .nil then do
        if self~isConnected then iterate
        leave
      end
    end
    else do
      m = self~receive(seconds)
      if m == .nil then leave
    end
    n = n + 1
    drop result
    block~call(m)
    if symbol('RESULT') == 'VAR' then if result == 0 then leave
  end
  return n

::method pending
  expose handle
  return MqttPending(handle)

::method ping
  expose handle
  return MqttPing(handle) = 0

::method isConnected
  expose handle
  return MqttPending(handle) >= 0

::method error
  return MqttError()

::method disconnect
  expose handle
  return MqttDisconnect(handle) = 0

::method string
  expose handle host port
  return 'an Mqtt connection to' host':'port '('handle')'

::class MqttMessage public
::attribute topic get
::attribute message get
::attribute qos get
::attribute retain get
::attribute dup get

::method init
  expose topic message qos retain dup
  use arg topic, message, qos, retain, dup
  if arg(3, 'o') then qos = 0
  if arg(4, 'o') then retain = 0
  if arg(5, 'o') then dup = 0

::method string
  expose topic message
  return topic'='message
