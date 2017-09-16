(define (problem l20)
  (:domain uts)
  (:objects n1 n2 n3 n4 n5 n6 n7 n8 n9 n10 n11 n12 n13 n14 n15 n16 n17 n18 n19 n20 - node)
  (:init (and (edge n1 n2) (edge n2 n1)
              (edge n2 n3) (edge n3 n2)
              (edge n3 n4) (edge n4 n3)
              (edge n4 n5) (edge n5 n4)
              (edge n5 n6) (edge n6 n5)
              (edge n6 n7) (edge n7 n6)
              (edge n7 n8) (edge n8 n7)
              (edge n8 n9) (edge n9 n8)
              (edge n9 n10) (edge n10 n9)
              (edge n10 n11) (edge n11 n10)
              (edge n11 n12) (edge n12 n11)
              (edge n12 n13) (edge n13 n12)
              (edge n13 n14) (edge n14 n13)
              (edge n14 n15) (edge n15 n14)
              (edge n15 n16) (edge n16 n15)
              (edge n16 n17) (edge n17 n16)
              (edge n17 n18) (edge n18 n17)
              (edge n18 n19) (edge n19 n18)
              (edge n19 n20) (edge n20 n19)
              (oneof (at n1) (at n2) (at n3) (at n4) (at n5) (at n6) (at n7) (at n8) (at n9) (at n10) (at n11) (at n12) (at n13) (at n14) (at n15) (at n16) (at n17) (at n18) (at n19) (at n20))
         )
  )
  (:goal (and (visited n1) (visited n2) (visited n3) (visited n4) (visited n5) (visited n6) (visited n7) (visited n8) (visited n9) (visited n10) (visited n11) (visited n12) (visited n13) (visited n14) (visited n15) (visited n16) (visited n17) (visited n18) (visited n19) (visited n20)))
)
