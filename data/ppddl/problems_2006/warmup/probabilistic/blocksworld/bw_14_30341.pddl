(define (problem bw_14_30341)
  (:domain blocks-domain)
  (:objects b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 - block)
  (:init (emptyhand) (on b1 b5) (on-table b2) (on b3 b6) (on-table b4) (on b5 b4) (on b6 b13) (on b7 b10) (on b8 b9) (on b9 b1) (on-table b10) (on-table b11) (on b12 b14) (on b13 b8) (on-table b14) (clear b2) (clear b3) (clear b7) (clear b11) (clear b12))
  (:goal (and (emptyhand) (on b1 b7) (on b2 b12) (on b3 b1) (on-table b4) (on-table b5) (on b6 b8) (on b7 b6) (on-table b8) (on b9 b10) (on b10 b5) (on-table b11) (on b12 b4) (on b13 b2) (on b14 b13) (clear b3) (clear b9) (clear b11) (clear b14)))
)
