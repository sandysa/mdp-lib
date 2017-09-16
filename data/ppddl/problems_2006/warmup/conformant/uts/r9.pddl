(define (problem r9)
  (:domain uts)
  (:objects n1 n2 n3 n4 n5 n6 n7 n8 n9 - node)
  (:init (and (edge n1 n5)
              (edge n1 n8)
              (edge n2 n1)
              (edge n2 n3)
              (edge n2 n5)
              (edge n2 n6)
              (edge n2 n9)
              (edge n3 n5)
              (edge n3 n6)
              (edge n3 n9)
              (edge n4 n1)
              (edge n4 n2)
              (edge n4 n5)
              (edge n4 n6)
              (edge n4 n7)
              (edge n4 n9)
              (edge n5 n1)
              (edge n5 n6)
              (edge n5 n7)
              (edge n5 n8)
              (edge n5 n9)
              (edge n6 n1)
              (edge n6 n2)
              (edge n6 n3)
              (edge n6 n5)
              (edge n6 n8)
              (edge n6 n9)
              (edge n7 n1)
              (edge n7 n2)
              (edge n7 n5)
              (edge n7 n6)
              (edge n7 n9)
              (edge n8 n4)
              (edge n8 n5)
              (edge n8 n7)
              (edge n8 n9)
              (edge n9 n1)
              (edge n9 n3)
              (edge n9 n4)
              (edge n9 n5)
              (edge n9 n8)
              (oneof (at n1) (at n2) (at n3) (at n4) (at n5) (at n6) (at n7) (at n8) (at n9))
         )
  )
  (:goal (and (visited n1) (visited n2) (visited n3) (visited n4) (visited n5) (visited n6) (visited n7) (visited n8) (visited n9)))
)
