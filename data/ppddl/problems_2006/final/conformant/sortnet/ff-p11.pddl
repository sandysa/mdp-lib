(define (problem s12)
  (:domain sortnet)
  (:objects l1 l2 l3 l4 l5 l6 l7 l8 l9 l10 l11 l12 - line)
  (:init (and (less l1 l2)
              (less l1 l3)
              (less l1 l4)
              (less l1 l5)
              (less l1 l6)
              (less l1 l7)
              (less l1 l8)
              (less l1 l9)
              (less l1 l10)
              (less l1 l11)
              (less l1 l12)
              (less l2 l3)
              (less l2 l4)
              (less l2 l5)
              (less l2 l6)
              (less l2 l7)
              (less l2 l8)
              (less l2 l9)
              (less l2 l10)
              (less l2 l11)
              (less l2 l12)
              (less l3 l4)
              (less l3 l5)
              (less l3 l6)
              (less l3 l7)
              (less l3 l8)
              (less l3 l9)
              (less l3 l10)
              (less l3 l11)
              (less l3 l12)
              (less l4 l5)
              (less l4 l6)
              (less l4 l7)
              (less l4 l8)
              (less l4 l9)
              (less l4 l10)
              (less l4 l11)
              (less l4 l12)
              (less l5 l6)
              (less l5 l7)
              (less l5 l8)
              (less l5 l9)
              (less l5 l10)
              (less l5 l11)
              (less l5 l12)
              (less l6 l7)
              (less l6 l8)
              (less l6 l9)
              (less l6 l10)
              (less l6 l11)
              (less l6 l12)
              (less l7 l8)
              (less l7 l9)
              (less l7 l10)
              (less l7 l11)
              (less l7 l12)
              (less l8 l9)
              (less l8 l10)
              (less l8 l11)
              (less l8 l12)
              (less l9 l10)
              (less l9 l11)
              (less l9 l12)
              (less l10 l11)
              (less l10 l12)
              (less l11 l12)
              (unknown (high l1))
              (unknown (high l2))
              (unknown (high l3))
              (unknown (high l4))
              (unknown (high l5))
              (unknown (high l6))
              (unknown (high l7))
              (unknown (high l8))
              (unknown (high l9))
              (unknown (high l10))
              (unknown (high l11))
              (unknown (high l12))
         )
  )
  (:goal (and (or (not (high l1)) (high l2))
              (or (not (high l2)) (high l3))
              (or (not (high l3)) (high l4))
              (or (not (high l4)) (high l5))
              (or (not (high l5)) (high l6))
              (or (not (high l6)) (high l7))
              (or (not (high l7)) (high l8))
              (or (not (high l8)) (high l9))
              (or (not (high l9)) (high l10))
              (or (not (high l10)) (high l11))
              (or (not (high l11)) (high l12))
         )
  )
)
