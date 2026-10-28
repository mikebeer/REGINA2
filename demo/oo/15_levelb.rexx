options levelb
/* 15_levelb.rexx - a cREXX Level B program run by Regina
 *
 * "options levelb" switches the cREXX notation on: interfaces with
 * factories, classes implementing them, attributes (_name = .type),
 * methods "name: method = .type", procedures, typed arguments and
 * declarations. .vehicle("...") asks every class implementing the
 * interface (their "match") which one should make the object.
 */
namespace garage expose vehicle

main: procedure = .int
  arg args = .string[]
  fleet = .array~new
  fleet~append(.vehicle("mini"))
  fleet~append(.vehicle("truck:7.5"))
  fleet~append(.vehicle.electric(60))
  do v over fleet
    say v.describe() 'wheels:' v.wheels() typeof(v)
  end
  say 'load of the fleet:' load(fleet) 't'
  t = fleet[2] as .vehicle
  say (t is .truck) (t is .car)
  count = .int
  say 'count starts at' count
  return 0

load: procedure = .float
  arg list = .vehicle[]
  total = 0
  do v over list
    if v is .truck then total = total + v.tons()
  end
  return total

vehicle: interface
  *: factory
  arg spec = .string
  electric: factory
  arg kwh = .int

  describe: method = .string
    return kind() || ":" || name()
  kind: method = .string
  name: method = .string
  wheels: method = .int

car: class implements .vehicle
  _name = .string
  _battery = .int

  *: factory
    arg spec = .string
    _name = spec
    return

  electric: factory
    arg kwh = .int
    _name = "e-car"
    _battery = kwh
    return

  kind: method = .string
    if _battery > 0 then return "car(" || _battery || " kWh)"
    return "car"
  name: method = .string
    return _name
  wheels: method = .int
    return 4

truck: class implements .vehicle
  _name = .string
  _tons = .float

  *: match
    arg spec = .string
    if left(spec, 6) = "truck:" then return 10
    return 0

  *: factory
    arg spec = .string
    parse var spec _name ":" _tons
    return

  kind: method = .string
    return "truck"
  name: method = .string
    return _name
  wheels: method = .int
    return 6
  tons: method = .float
    return _tons
