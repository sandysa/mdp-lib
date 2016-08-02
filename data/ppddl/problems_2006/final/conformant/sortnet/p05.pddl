(define (problem s6)
  (:domain sortnet)
  (:objects l1 l2 l3 l4 l5 l6 - line)
  (:init (and (less l1 l2)
              (less l1 l3)
              (less l1 l4)
              (less l1 l5)
              (less l1 l6)
              (less l2 l3)
              (less l2 l4)
              (less l2 l5)
              (less l2 l6)
              (less l3 l4)
              (less l3 l5)
              (less l3 l6)
              (less l4 l5)
              (less l4 l6)
              (less l5 l6)
              (oneof (high l1) (not (high l1)))
              (oneof (high l2) (not (high l2)))
              (oneof (high l3) (not (high l3)))
              (oneof (high l4) (not (high l4)))
              (oneof (high l5) (not (high l5)))
              (oneof (high l6) (not (high l6)))
         )
  )
  (:goal (and (or (not (high l1)) (high l2))
              (or (not (high l2)) (high l3))
              (or (not (high l3)) (high l4))
              (or (not (high l4)) (high l5))
              (or (not (high l5)) (high l6))
         )
  )
)
