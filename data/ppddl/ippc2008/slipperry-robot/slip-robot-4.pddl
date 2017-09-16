(define (domain slip-robot)
  (:requirements :typing :strips :equality :probabilistic-effects :rewards)
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
    :effect 
	(probabilistic
        0.6 (and (vehicle-at ?to) (not(vehicle-at ?from))(has-umbrella))
        0.4 (and (vehicle-at ?from) (has-umbrella)))
)

(:action move-car-without-umbrella
    :parameters (?from ?to - location)
    :precondition (and (vehicle-at ?from) (road ?from ?to))
    :effect
      (probabilistic
        0.4 (and (vehicle-at ?to) (not(vehicle-at ?from)))
        0.6 (vehicle-at ?from))
  )
 (:action move-car-umbrella
    :parameters (?from ?to - location)
    :precondition (and (vehicle-at ?from) (road ?from ?to) (has-umbrella))
    :effect
      (probabilistic
        0.8 (and (vehicle-at ?to) (not(vehicle-at ?from)))
        0.2 (vehicle-at ?from))
  )
)

(define (problem slip-robot-4)
                   (:domain slip-robot)
                   (:objects l-1-1 l-1-2 l-1-3 l-1-4 l-1-5 l-2-1 l-2-2 l-2-3 l-2-4 l-2-5 l-3-1 l-3-2 l-3-3 l-3-4 l-3-5 l-4-1 l-4-2 l-4-3 l-4-4 l-4-5 l-5-1 l-5-2 l-5-3 l-5-4 l-5-5 - location)
                   (:init (vehicle-at l-5-2) (road l-1-1 l-1-2) (road l-1-2 l-1-1) (road l-1-2 l-1-3) (road l-1-3 l-1-2)(road l-1-3 l-1-4) (road l-1-4 l-1-3)
				   (road l-1-4 l-1-5) (road l-1-5 l-1-4) (road l-2-1 l-2-2) (road l-2-2 l-2-1) (road l-2-2 l-2-3) (road l-2-3 l-2-2)(road l-2-3 l-2-4) (road l-2-4 l-2-3)(road l-2-4 l-2-5) (road l-2-5 l-2-4) (road l-3-1 l-3-2) (road l-3-2 l-3-1) (road l-3-2 l-3-3) (road l-3-3 l-3-2)(road l-3-3 l-3-4) (road l-3-4 l-3-3)(road l-3-4 l-3-5) (road l-3-5 l-3-4) (road l-4-1 l-4-2) (road l-4-2 l-4-1) (road l-4-2 l-4-3) (road l-4-3 l-4-2)(road l-4-3 l-4-4) (road l-4-4 l-4-3)(road l-4-4 l-4-5) (road l-4-5 l-4-4) (road l-5-1 l-5-2) (road l-5-2 l-5-1) (road l-5-2 l-5-3) (road l-5-3 l-5-2)(road l-5-3 l-5-4) (road l-5-4 l-5-3)(road l-5-4 l-5-5) (road l-5-5 l-5-4)(road l-1-1 l-2-1) (road l-2-1 l-1-1) (road l-2-1 l-3-1) (road l-3-1 l-2-1) (road l-3-1 l-4-1) (road l-4-1 l-3-1) (road l-4-1 l-5-1) (road l-5-1 l-4-1) (road l-1-2 l-2-2) (road l-2-2 l-1-2) (road l-2-2 l-3-2) (road l-3-2 l-2-2) (road l-3-2 l-4-2) (road l-4-2 l-3-2) (road l-4-2 l-5-2) (road l-5-2 l-4-2) (road l-1-3 l-2-3) (road l-2-3 l-1-3) (road l-2-3 l-3-3) (road l-3-3 l-2-3) (road l-3-3 l-4-3) (road l-4-3 l-3-3) (road l-4-3 l-5-3) (road l-5-3 l-4-3) (road l-1-4 l-2-4) (road l-2-4 l-1-4) (road l-2-4 l-3-4) (road l-3-4 l-2-4) (road l-3-4 l-4-4) (road l-4-4 l-3-4) (road l-4-4 l-5-4) (road l-5-4 l-4-4) (road l-1-5 l-2-5) (road l-2-5 l-1-5) (road l-2-5 l-3-5) (road l-3-5 l-2-5) (road l-3-5 l-4-5) (road l-4-5 l-3-5) (road l-4-5 l-5-5) (road l-5-5 l-4-5)(road l-5-2 l-4-3) (road l-4-3 l-5-2) (road l-4-3 l-3-4)(road l-3-4 l-4-3) (road l-3-4 l-2-5)(road l-2-5 l-3-4)(umbrella-in l-1-1)(umbrella-in l-1-4)(umbrella-in l-1-5)(umbrella-in l-2-2)(umbrella-in l-3-2)(umbrella-in l-3-4)(umbrella-in l-3-5)(umbrella-in l-4-1)(umbrella-in l-4-5)(umbrella-in l-5-2)(umbrella-in l-5-4))
                   (:goal (vehicle-at l-4-5)) (:goal-reward 100) (:metric maximize (reward)))

