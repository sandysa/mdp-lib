(define (problem ex_bw_12_5182)
  (:domain exploding-blocksworld-domain)
  (:objects b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 - block)
  (:init (emptyhand) (on b1 b7) (on-table b2) (on b3 b1) (on b4 b8) (on b5 b3) (on b6 b4) (on b7 b11) (on b8 b9) (on-table b9) (on b10 b12) (on b11 b10) (on b12 b6) (clear b2) (clear b5) (no-detonated b1) (no-destroyed b1) (no-detonated b2) (no-destroyed b2) (no-detonated b3) (no-destroyed b3) (no-detonated b4) (no-destroyed b4) (no-detonated b5) (no-destroyed b5) (no-detonated b6) (no-destroyed b6) (no-detonated b7) (no-destroyed b7) (no-detonated b8) (no-destroyed b8) (no-detonated b9) (no-destroyed b9) (no-detonated b10) (no-destroyed b10) (no-detonated b11) (no-destroyed b11) (no-detonated b12) (no-destroyed b12) (no-destroyed-table))
  (:goal (and (emptyhand) (on b1 b6) (on b2 b9) (on-table b3) (on b4 b5) (on b5 b10) (on-table b6) (on-table b7) (on b8 b2) (on-table b9) (on b10 b3) (on b11 b1) (on b12 b7)))
)
