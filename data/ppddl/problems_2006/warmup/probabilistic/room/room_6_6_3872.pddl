(define (domain room_6_6_3872)
  (:requirements :typing :probabilistic-effects :conditional-effects)
  (:types level db)
  (:constants X1 X2 X3 X4 X5 X6 - pos_x Y1 Y2 Y3 Y4 Y5 Y6 - pos_y)
  (:predicates (at ?x - pos_x ?y - pos_y))

  (:action up_1_1
    :precondition (at X1 Y1)
    :effect (and (at L1) (not (at L0)))
  )
)

(define (problem room_6_6_3872)
  (:domain room_6_6_3872)
  (:init (at X1 Y1))
  (:goal (at X3 Y3))
)

