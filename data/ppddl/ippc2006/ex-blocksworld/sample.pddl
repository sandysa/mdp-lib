(define (problem ex_bw_4_2187)
  (:domain exploding-blocksworld)
  (:objects b1 b2 b3 b4 - block)
  (:init (emptyhand) (on b1 b2) (on-table b2) (on b3 b4) (on b4 b1) (clear b3) (no-detonated b1) (no-destroyed b1) (no-detonated b2) (no-destroyed b2) (no-detonated b3) (no-destroyed b3) (no-detonated b4) (no-destroyed b4) (no-destroyed-table))
  (:goal (and (on b1 b4) (on-table b3)))
)
