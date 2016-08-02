(define (domain slip-robot)
  (:requirements :strips :equality)
  (:types location)
  (:predicates (vehicle-at ?loc - location)
	       (umbrella-in ?loc - location)
	       (road ?from - location ?to - location)
	       (has-umbrella))
   
  (:action pickumbrella
    :parameters (?loc - location)
    :precondition (and (not(has-umbrella)) (umbrella-in ?loc) (vehicle-at ?loc))
    :effect (has-umbrella))

(:action pickumbrella-and-move-with-umbrella
    :parameters (?from ?to - location)
    :precondition (and (not(has-umbrella)) (umbrella-in ?from) (vehicle-at ?from) (road ?from ?to))
    :effect (and (vehicle-at ?to) (not(vehicle-at ?from))(has-umbrella))
)

(:action move-car-without-umbrella
    :parameters (?from ?to - location)
    :precondition (and (vehicle-at ?from) (road ?from ?to))
    :effect (vehicle-at ?from)
  )
 (:action move-car-umbrella
    :parameters (?from ?to - location)
    :precondition (and (vehicle-at ?from) (road ?from ?to) (has-umbrella))
    :effect (and (vehicle-at ?to) (not(vehicle-at ?from)))
)
)