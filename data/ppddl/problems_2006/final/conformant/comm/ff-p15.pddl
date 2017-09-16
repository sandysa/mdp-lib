(define (problem comm_19_16)
  (:domain comm)
  (:objects s1 s2 s3 s4 s5 s6 s7 s8 s9 s10 s11 s12 s13 s14 s15 s16 s17 s18 - stage p0 p1 p2 p3 p4 p5 p6 p7 p8 p9 p10 p11 p12 p13 p14 p15 - packet)
  (:init (and (current-stage s0)
              (in-channel p0) (seq-number p0 s0) (unknown (noisy p0))
              (in-channel p1) (seq-number p1 s1) (unknown (noisy p1))
              (in-channel p2) (seq-number p2 s2) (unknown (noisy p2))
              (in-channel p3) (seq-number p3 s3) (unknown (noisy p3))
              (in-channel p4) (seq-number p4 s4) (unknown (noisy p4))
              (in-channel p5) (seq-number p5 s5) (unknown (noisy p5))
              (in-channel p6) (seq-number p6 s6) (unknown (noisy p6))
              (in-channel p7) (seq-number p7 s7) (unknown (noisy p7))
              (in-channel p8) (seq-number p8 s8) (unknown (noisy p8))
              (in-channel p9) (seq-number p9 s9) (unknown (noisy p9))
              (in-channel p10) (seq-number p10 s10) (unknown (noisy p10))
              (in-channel p11) (seq-number p11 s11) (unknown (noisy p11))
              (in-channel p12) (seq-number p12 s12) (unknown (noisy p12))
              (in-channel p13) (seq-number p13 s13) (unknown (noisy p13))
              (in-channel p14) (seq-number p14 s14) (unknown (noisy p14))
              (in-channel p15) (seq-number p15 s15) (unknown (noisy p15))
              (next-stage s0 s1)
              (next-stage s1 s2)
              (next-stage s2 s3)
              (next-stage s3 s4)
              (next-stage s4 s5)
              (next-stage s5 s6)
              (next-stage s6 s7)
              (next-stage s7 s8)
              (next-stage s8 s9)
              (next-stage s9 s10)
              (next-stage s10 s11)
              (next-stage s11 s12)
              (next-stage s12 s13)
              (next-stage s13 s14)
              (next-stage s14 s15)
              (next-stage s15 s16)
              (next-stage s16 s17)
              (next-stage s17 s18)
         )
  )
  (:goal (and (ok p0) (ok p1) (ok p2) (ok p3) (ok p4) (ok p5) (ok p6) (ok p7) (ok p8) (ok p9) (ok p10) (ok p11) (ok p12) (ok p13) (ok p14) (ok p15)))
)
