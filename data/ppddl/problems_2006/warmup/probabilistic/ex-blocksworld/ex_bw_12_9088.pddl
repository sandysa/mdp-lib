(define (problem ex_bw_12_9088)
  (:domain exploding-blocksworld-domain)
  (:objects b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 - block)
  (:init (emptyhand) (on b1 b8) (on b2 b10) (on-table b3) (on b4 b11) (on-table b5) (on b6 b4) (on b7 b1) (on b8 b12) (on b9 b2) (on-table b10) (on-table b11) (on-table b12) (clear b3) (clear b5) (clear b6) (clear b7) (clear b9) (no-detonated b1) (no-destroyed b1) (no-detonated b2) (no-destroyed b2) (no-detonated b3) (no-destroyed b3) (no-detonated b4) (no-destroyed b4) (no-detonated b5) (no-destroyed b5) (no-detonated b6) (no-destroyed b6) (no-detonated b7) (no-destroyed b7) (no-detonated b8) (no-destroyed b8) (no-detonated b9) (no-destroyed b9) (no-detonated b10) (no-destroyed b10) (no-detonated b11) (no-destroyed b11) (no-detonated b12) (no-destroyed b12) (no-destroyed-table))
  (:goal (and (emptyhand) (on b1 b4) (on b2 b5) (on b3 b6) (on-table b4) (on b5 b12) (on b6 b9) (on-table b7) (on b8 b10) (on b9 b7) (on b10 b2) (on b11 b3) (on-table b12)))
)
