(define (domain tree_6_3_4)
  (:requirements :typing :probabilistic-effects :conditional-effects)
  (:types level db)
  (:constants L0 L1 L2 L3 L4 L5 L6 - level db0 db1 db2 db3 db4 db5 db6 - db)
  (:predicates (at ?l - level) (bit ?l - level) (noise ?d - db))

  (:action left_0_0
    :precondition (and (at L0) (noise db0))
    :effect (and (at L1) (not (at L0)))
  )
  (:action left_1_0
    :precondition (and (at L1) (noise db0))
    :effect (and (at L2) (not (at L1)))
  )
  (:action left_1_1
    :precondition (and (at L1) (noise db1))
    :effect (probabilistic
               3/4 (and (at L2) (not (at L1)))
               1/4 (and (at L0) (not (at L1)) (when (bit L1) (and (not (bit L1)) (noise db0) (not (noise db1))))))
  )
  (:action left_2_0
    :precondition (and (at L2) (noise db0))
    :effect (and (at L3) (not (at L2)))
  )
  (:action left_2_1
    :precondition (and (at L2) (noise db1))
    :effect (probabilistic
               3/4 (and (at L3) (not (at L2)))
               1/4 (and (at L1) (not (at L2)) (when (bit L2) (and (not (bit L2)) (noise db0) (not (noise db1))))))
  )
  (:action left_2_2
    :precondition (and (at L2) (noise db2))
    :effect (probabilistic
               9/16 (and (at L3) (not (at L2)))
               7/16 (and (at L1) (not (at L2)) (when (bit L2) (and (not (bit L2)) (noise db1) (not (noise db2))))))
  )
  (:action left_3_0
    :precondition (and (at L3) (noise db0))
    :effect (and (at L4) (not (at L3)))
  )
  (:action left_3_1
    :precondition (and (at L3) (noise db1))
    :effect (probabilistic
               3/4 (and (at L4) (not (at L3)))
               1/4 (and (at L2) (not (at L3)) (when (bit L3) (and (not (bit L3)) (noise db0) (not (noise db1))))))
  )
  (:action left_3_2
    :precondition (and (at L3) (noise db2))
    :effect (probabilistic
               9/16 (and (at L4) (not (at L3)))
               7/16 (and (at L2) (not (at L3)) (when (bit L3) (and (not (bit L3)) (noise db1) (not (noise db2))))))
  )
  (:action left_3_3
    :precondition (and (at L3) (noise db3))
    :effect (probabilistic
               42187/100000 (and (at L4) (not (at L3)))
               57813/100000 (and (at L2) (not (at L3)) (when (bit L3) (and (not (bit L3)) (noise db2) (not (noise db3))))))
  )
  (:action left_4_0
    :precondition (and (at L4) (noise db0))
    :effect (and (at L5) (not (at L4)))
  )
  (:action left_4_1
    :precondition (and (at L4) (noise db1))
    :effect (probabilistic
               3/4 (and (at L5) (not (at L4)))
               1/4 (and (at L3) (not (at L4)) (when (bit L4) (and (not (bit L4)) (noise db0) (not (noise db1))))))
  )
  (:action left_4_2
    :precondition (and (at L4) (noise db2))
    :effect (probabilistic
               9/16 (and (at L5) (not (at L4)))
               7/16 (and (at L3) (not (at L4)) (when (bit L4) (and (not (bit L4)) (noise db1) (not (noise db2))))))
  )
  (:action left_4_3
    :precondition (and (at L4) (noise db3))
    :effect (probabilistic
               42187/100000 (and (at L5) (not (at L4)))
               57813/100000 (and (at L3) (not (at L4)) (when (bit L4) (and (not (bit L4)) (noise db2) (not (noise db3))))))
  )
  (:action left_4_4
    :precondition (and (at L4) (noise db4))
    :effect (probabilistic
               791/2500 (and (at L5) (not (at L4)))
               1709/2500 (and (at L3) (not (at L4)) (when (bit L4) (and (not (bit L4)) (noise db3) (not (noise db4))))))
  )
  (:action left_5_0
    :precondition (and (at L5) (noise db0))
    :effect (and (at L6) (not (at L5)))
  )
  (:action left_5_1
    :precondition (and (at L5) (noise db1))
    :effect (probabilistic
               3/4 (and (at L6) (not (at L5)))
               1/4 (and (at L4) (not (at L5)) (when (bit L5) (and (not (bit L5)) (noise db0) (not (noise db1))))))
  )
  (:action left_5_2
    :precondition (and (at L5) (noise db2))
    :effect (probabilistic
               9/16 (and (at L6) (not (at L5)))
               7/16 (and (at L4) (not (at L5)) (when (bit L5) (and (not (bit L5)) (noise db1) (not (noise db2))))))
  )
  (:action left_5_3
    :precondition (and (at L5) (noise db3))
    :effect (probabilistic
               42187/100000 (and (at L6) (not (at L5)))
               57813/100000 (and (at L4) (not (at L5)) (when (bit L5) (and (not (bit L5)) (noise db2) (not (noise db3))))))
  )
  (:action left_5_4
    :precondition (and (at L5) (noise db4))
    :effect (probabilistic
               791/2500 (and (at L6) (not (at L5)))
               1709/2500 (and (at L4) (not (at L5)) (when (bit L5) (and (not (bit L5)) (noise db3) (not (noise db4))))))
  )
  (:action left_5_5
    :precondition (and (at L5) (noise db5))
    :effect (probabilistic
               2373/10000 (and (at L6) (not (at L5)))
               7627/10000 (and (at L4) (not (at L5)) (when (bit L5) (and (not (bit L5)) (noise db4) (not (noise db5))))))
  )

  (:action right_0_0
    :precondition (and (at L0) (noise db0))
    :effect (and (at L1) (not (at L0)) (noise db1) (not (noise db0)) (bit L1))
  )
  (:action right_1_0
    :precondition (and (at L1) (noise db0))
    :effect (and (at L2) (not (at L1)) (noise db1) (not (noise db0)) (bit L2))
  )
  (:action right_1_1
    :precondition (and (at L1) (noise db1))
    :effect (probabilistic
               3/4 (and (at L2) (not (at L1)) (noise db2) (not (noise db1)) (bit L2))
               1/4 (and (at L0) (not (at L1)) (when (bit L1) (and (not (bit L1)) (noise db0) (not (noise db1))))))
  )
  (:action right_2_0
    :precondition (and (at L2) (noise db0))
    :effect (and (at L3) (not (at L2)) (noise db1) (not (noise db0)) (bit L3))
  )
  (:action right_2_1
    :precondition (and (at L2) (noise db1))
    :effect (probabilistic
               3/4 (and (at L3) (not (at L2)) (noise db2) (not (noise db1)) (bit L3))
               1/4 (and (at L1) (not (at L2)) (when (bit L2) (and (not (bit L2)) (noise db0) (not (noise db1))))))
  )
  (:action right_2_2
    :precondition (and (at L2) (noise db2))
    :effect (probabilistic
               9/16 (and (at L3) (not (at L2)) (noise db3) (not (noise db2)) (bit L3))
               7/16 (and (at L1) (not (at L2)) (when (bit L2) (and (not (bit L2)) (noise db1) (not (noise db2))))))
  )
  (:action right_3_0
    :precondition (and (at L3) (noise db0))
    :effect (and (at L4) (not (at L3)) (noise db1) (not (noise db0)) (bit L4))
  )
  (:action right_3_1
    :precondition (and (at L3) (noise db1))
    :effect (probabilistic
               3/4 (and (at L4) (not (at L3)) (noise db2) (not (noise db1)) (bit L4))
               1/4 (and (at L2) (not (at L3)) (when (bit L3) (and (not (bit L3)) (noise db0) (not (noise db1))))))
  )
  (:action right_3_2
    :precondition (and (at L3) (noise db2))
    :effect (probabilistic
               9/16 (and (at L4) (not (at L3)) (noise db3) (not (noise db2)) (bit L4))
               7/16 (and (at L2) (not (at L3)) (when (bit L3) (and (not (bit L3)) (noise db1) (not (noise db2))))))
  )
  (:action right_3_3
    :precondition (and (at L3) (noise db3))
    :effect (probabilistic
               42187/100000 (and (at L4) (not (at L3)) (noise db4) (not (noise db3)) (bit L4))
               57813/100000 (and (at L2) (not (at L3)) (when (bit L3) (and (not (bit L3)) (noise db2) (not (noise db3))))))
  )
  (:action right_4_0
    :precondition (and (at L4) (noise db0))
    :effect (and (at L5) (not (at L4)) (noise db1) (not (noise db0)) (bit L5))
  )
  (:action right_4_1
    :precondition (and (at L4) (noise db1))
    :effect (probabilistic
               3/4 (and (at L5) (not (at L4)) (noise db2) (not (noise db1)) (bit L5))
               1/4 (and (at L3) (not (at L4)) (when (bit L4) (and (not (bit L4)) (noise db0) (not (noise db1))))))
  )
  (:action right_4_2
    :precondition (and (at L4) (noise db2))
    :effect (probabilistic
               9/16 (and (at L5) (not (at L4)) (noise db3) (not (noise db2)) (bit L5))
               7/16 (and (at L3) (not (at L4)) (when (bit L4) (and (not (bit L4)) (noise db1) (not (noise db2))))))
  )
  (:action right_4_3
    :precondition (and (at L4) (noise db3))
    :effect (probabilistic
               42187/100000 (and (at L5) (not (at L4)) (noise db4) (not (noise db3)) (bit L5))
               57813/100000 (and (at L3) (not (at L4)) (when (bit L4) (and (not (bit L4)) (noise db2) (not (noise db3))))))
  )
  (:action right_4_4
    :precondition (and (at L4) (noise db4))
    :effect (probabilistic
               791/2500 (and (at L5) (not (at L4)) (noise db5) (not (noise db4)) (bit L5))
               1709/2500 (and (at L3) (not (at L4)) (when (bit L4) (and (not (bit L4)) (noise db3) (not (noise db4))))))
  )
  (:action right_5_0
    :precondition (and (at L5) (noise db0))
    :effect (and (at L6) (not (at L5)) (noise db1) (not (noise db0)) (bit L6))
  )
  (:action right_5_1
    :precondition (and (at L5) (noise db1))
    :effect (probabilistic
               3/4 (and (at L6) (not (at L5)) (noise db2) (not (noise db1)) (bit L6))
               1/4 (and (at L4) (not (at L5)) (when (bit L5) (and (not (bit L5)) (noise db0) (not (noise db1))))))
  )
  (:action right_5_2
    :precondition (and (at L5) (noise db2))
    :effect (probabilistic
               9/16 (and (at L6) (not (at L5)) (noise db3) (not (noise db2)) (bit L6))
               7/16 (and (at L4) (not (at L5)) (when (bit L5) (and (not (bit L5)) (noise db1) (not (noise db2))))))
  )
  (:action right_5_3
    :precondition (and (at L5) (noise db3))
    :effect (probabilistic
               42187/100000 (and (at L6) (not (at L5)) (noise db4) (not (noise db3)) (bit L6))
               57813/100000 (and (at L4) (not (at L5)) (when (bit L5) (and (not (bit L5)) (noise db2) (not (noise db3))))))
  )
  (:action right_5_4
    :precondition (and (at L5) (noise db4))
    :effect (probabilistic
               791/2500 (and (at L6) (not (at L5)) (noise db5) (not (noise db4)) (bit L6))
               1709/2500 (and (at L4) (not (at L5)) (when (bit L5) (and (not (bit L5)) (noise db3) (not (noise db4))))))
  )
  (:action right_5_5
    :precondition (and (at L5) (noise db5))
    :effect (probabilistic
               2373/10000 (and (at L6) (not (at L5)) (noise db6) (not (noise db5)) (bit L6))
               7627/10000 (and (at L4) (not (at L5)) (when (bit L5) (and (not (bit L5)) (noise db4) (not (noise db5))))))
  )
)

(define (problem tree_6_3_4)
  (:domain tree_6_3_4)
  (:init (at L0) (noise db0))
  (:goal (at L6))
)

