(define (problem slip-robot-1)
                   (:domain slip-robot)
                   (:objects l-1-1 l-1-2 l-1-3 l-2-1 l-2-2 l-2-3 l-3-1 l-3-2 l-3-3 - location)
                   (:init (vehicle-at l-1-1)(road l-1-1 l-1-2)(road l-1-2 l-1-3)(road l-1-1 l-2-1)(road l-1-2 l-2-2)(road l-2-1 l-1-2)(road l-2-2 l-1-3) (road l-2-1 l-3-1)(road l-3-1 l-2-2)(road l-3-1 l-3-2)(road l-2-1 l-2-2)(road l-2-2 l-3-3)(road l-2-2 l-2-3)(road l-2-3 l-3-3)(road l-3-2 l-3-3)(road l-2-2 l-3-2) (road l-1-2 l-1-1)(road l-1-3 l-1-2)(road l-2-1 l-1-1)(road l-2-2 l-1-2)(road l-1-2 l-2-1)(road l-1-3 l-2-2) (road l-3-1 l-2-1)(road l-2-2 l-3-1)(road l-3-2 l-3-1)(road l-2-2 l-2-1)(road l-3-3 l-2-2)(road l-2-3 l-2-2)(road l-3-3 l-2-3)(road l-3-3 l-3-2)(road l-3-2 l-2-2) (umbrella-in l-2-1)(umbrella-in l-2-2) (umbrella-in l-3-1) (umbrella-in l-3-2)(umbrella-in l-2-3)(umbrella-in l-3-3))
                   (:goal (vehicle-at l-1-3)) )
