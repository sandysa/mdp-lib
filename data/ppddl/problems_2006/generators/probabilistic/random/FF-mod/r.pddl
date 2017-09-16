(define (domain random_domain) 
 (:predicates 
	 (ZQ ?x ?y) 
	 (QT ?x ?y) 
	 (DV ?x ?y) 
	 (OI ?x ?y) 
	 (LK ?x ?y) 
	 (QF ?x ?y) 
	 (BY ?x ?y) 
	 (WL ?x ?y) 
	 (DX ?x ?y) 
	 (WS ?x ?y) 
         (goal-achieved)
 )
 (:action AFM
  :parameters (?X ?Y ?Z) 
  :precondition (and 	 (WS ?Y ?Z) 
	 (QF ?X ?Y) 
	 (DX ?Z ?X) 
  )
  :effect            (and (DV ?X ?Z) (not (WS ?Y ?Z))          ))
 (:action AUA
  :parameters (?X ?Y ?Z) 
  :precondition (and 	 (OI ?Y ?Z) 
	 (WL ?Z ?X) 
	 (ZQ ?X ?Y) 
  )
  :effect            (and (OI ?Z ?X) (not (WL ?Z ?X))          ))
 (:action ZXF
  :parameters (?X ?Y ?Z) 
  :precondition (and 	 (DX ?Y ?X) 
	 (DV ?Y ?Z) 
	 (QF ?X ?Y) 
  )
  :effect            (and (OI ?X ?Z) (not (DX ?Y ?X))          ))
 (:action PQB
  :parameters (?X ?Y ?Z) 
  :precondition (and 	 (DV ?X ?Y) 
	 (QT ?Y ?Z) 
	 (LK ?Z ?X) 
  )
  :effect            (and (LK ?Y ?Z) (not (LK ?Z ?X))          ))
 (:action SPP
  :parameters (?X ?Y ?Z) 
  :precondition (and 	 (DV ?X ?Z) 
	 (ZQ ?Z ?X) 
	 (WS ?X ?Y) 
  )
  :effect            (and (WL ?Y ?Z) (not (DV ?X ?Z))          ))
 (:action CLY
  :parameters (?X ?Y ?Z) 
  :precondition (and 	 (DX ?Z ?X) 
	 (OI ?Z ?Y) 
	 (QF ?Z ?Y) 
  )
  :effect            (and (BY ?X ?Y) (not (QF ?Z ?Y))          ))
 (:action PLM
  :parameters (?X ?Y ?Z) 
  :precondition (and 	 (DX ?Y ?Z) 
	 (WL ?Z ?Y) 
	 (ZQ ?Z ?X) 
  )
  :effect            (and (LK ?Z ?Y) (not (WL ?Z ?Y))          ))
 (:action XFG
  :parameters (?X ?Y ?Z) 
  :precondition (and 	 (ZQ ?X ?Z) 
	 (LK ?X ?Y) 
	 (LK ?X ?Z) 
  )
  :effect            (and (QT ?Z ?X) (not (LK ?X ?Y))          ))
 (:action AMT
  :parameters (?X ?Y ?Z) 
  :precondition (and 	 (QF ?Y ?X) 
	 (DX ?X ?Z) 
	 (WL ?Z ?Y) 
  )
  :effect            (and (WL ?Z ?Y) (not (DX ?X ?Z))          ))
 (:action BZZ
  :parameters (?X ?Y ?Z) 
  :precondition (and 	 (ZQ ?X ?Z) 
	 (ZQ ?Z ?Y) 
	 (OI ?Z ?X) 
  )
  :effect            (and (QF ?Z ?X) (not (OI ?Z ?X))          ))
 (:action LRA
  :parameters (?X ?Y ?Z) 
  :precondition (and 	 (LK ?X ?Y) 
	 (QF ?Z ?Y) 
	 (BY ?X ?Y) 
  )
  :effect            (and (QF ?Z ?Y) (not (BY ?X ?Y))          ))
 (:action LEG
  :parameters (?X ?Y ?Z) 
  :precondition (and 	 (WS ?Z ?X) 
	 (LK ?Y ?X) 
	 (DX ?X ?Y) 
  )
  :effect            (and (ZQ ?Z ?X) (not (DX ?X ?Y))          ))
 (:action PJB
  :parameters (?X ?Y ?Z) 
  :precondition (and 	 (OI ?Z ?Y) 
	 (DX ?Y ?Z) 
	 (QT ?X ?Z) 
  )
  :effect            (and (DX ?Y ?X) (not (OI ?Z ?Y))          ))
 (:action NQP
  :parameters (?X ?Y ?Z) 
  :precondition (and 	 (BY ?Y ?X) 
	 (QT ?Z ?Y) 
	 (BY ?Y ?Z) 
  )
  :effect            (and (LK ?Z ?X) (not (BY ?Y ?X))          ))
 (:action BYB
  :parameters (?X ?Y ?Z) 
  :precondition (and 	 (WL ?X ?Y) 
	 (QF ?Y ?X) 
	 (QT ?Y ?Z) 
  )
  :effect            (and (BY ?Y ?Z) (not (QT ?Y ?Z))          ))
 (:action OAO
  :parameters (?X ?Y ?Z) 
  :precondition (and 	 (ZQ ?X ?Z) 
	 (WL ?Y ?X) 
	 (ZQ ?Z ?Y) 
  )
  :effect            (and (WS ?Z ?Y) (not (WL ?Y ?X))          ))
 (:action SSE
  :parameters (?X ?Y ?Z) 
  :precondition (and 	 (DV ?Y ?Z) 
	 (DX ?Z ?X) 
	 (QT ?Y ?X) 
  )
  :effect            (and (WS ?X ?Z) (not (DV ?Y ?Z))          ))
 (:action EUC
  :parameters (?X ?Y ?Z) 
  :precondition (and 	 (QT ?X ?Y) 
	 (QT ?Y ?Z) 
	 (BY ?Z ?Y) 
  )
  :effect            (and (QT ?Z ?Y) (not (QT ?X ?Y))          ))
 (:action BFK
  :parameters (?X ?Y ?Z) 
  :precondition (and 	 (WS ?X ?Y) 
	 (BY ?X ?Y) 
	 (WS ?Y ?X) 
  )
  :effect            (and (WL ?Z ?X) (not (BY ?X ?Y))          ))
 (:action OPU
  :parameters (?X ?Y ?Z) 
  :precondition (and 	 (DX ?X ?Y) 
	 (WL ?Y ?Z) 
	 (QT ?Z ?X) 
  )
  :effect            (and (WL ?Y ?X) (not (DX ?X ?Y))          ))
 (:action MMM
  :effect
    (and (goal-achieved))))
