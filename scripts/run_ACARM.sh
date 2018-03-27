#!/bin/bash

nsims=100
verbosity=0
heur=hmin

# tracks=(known/square-4-error known/square-5-error \
#         known/ring-5-error known/ring-6-error)
#  tracks=(known/square-4-error known/ring-5-error)
    tracks=(small)
#  tracks=(known/square-4-error) 
#   tracks=(known/ring-6-error)        
# ######## Racetrack domain problems # ########
     for track in ${tracks[@]}; do
#   echo "${track}|lrtdp"
#   ../testsolver.out --track=../data/tracks/$track.track \
#   --algorithm=lrtdp --v=$verbosity --n=1 \
#   --heuristic=$heur
#  
	#LAO
#         echo "********************************************************"
#         echo "${track}|lao"
#         ../testsolver.out --track=../data/tracks/$track.track \
#         --algorithm=lao --v=$verbosity --n=$nsims \
#        --heuristic=$heur
       
  

        #DET
      echo "********************************************************"
    echo "${track}|DET"
    ../testsolver.out --track=../data/tracks/$track.track \
    --algorithm=det --v=$verbosity --n=$nsims \
       --heuristic=$heur
       
       #ALLD
#    echo "********************************************************"
#     echo "${track}|alld"
#     ../testsolver.out --track=../data/tracks/$track.track \
#     --algorithm=alld --v=$verbosity --n=$nsims \
#        --heuristic=$heur
       
        #PRM
#     echo "********************************************************"
#     echo "${track}|prm"
#     ../testsolver.out --track=../data/tracks/$track.track \
#     --algorithm=prm --v=$verbosity --n=$nsims \
#        --heuristic=$heur
       
     #ACARM
#     echo "********************************************************"
#     echo "${track}|acarm"
#     ../testsolver.out --track=../data/tracks/$track.track \
#     --algorithm=acarm --v=$verbosity --n=$nsims \
#        --heuristic=$heur
       
   # FLARES
#   for horizon in `seq 0 1`; do
#     echo "${track}|flares(${horizon})"
#       ../testsolver.out --track=../data/tracks/$track.track \
#       --algorithm=flares --horizon=$horizon --v=$verbosity --n=$nsims \
#       --heuristic=$heur
#    done
#       
#   # FLARES-PROB
#   echo "${track}|flares-prob(0.25)"
#   ../testsolver.out --track=../data/tracks/$track.track \
#   --algorithm=flares --use-prob-depth --prob=0.25 --v=$verbosity --n=$nsims \
#   --heuristic=$heur
# 
#   # HDP(0)
#   echo "${track}|hdp(0)"
#   ../testsolver.out --track=../data/tracks/$track.track \
#   --algorithm=hdp --i=0 --v=$verbosity --n=$nsims \
#   --heuristic=$heur
# 
#   # HDP(0,0)
#   echo "${track}|hdp(0,0)"
#   ../testsolver.out --track=../data/tracks/$track.track \
#   --algorithm=hdp --i=0 --j=0 --v=$verbosity --n=$nsims \
#   --heuristic=$heur
# 
#   # SSiPP(t)
#   t=2
#   for i in `seq 0 1`; do
#     echo "${track}|ssipp(${t})"
#       ../testsolver.out --track=../data/tracks/$track.track \
#       --algorithm=ssipp --horizon=$t --v=$verbosity --n=$nsims \
#       --heuristic=$heur
#     let "t *= 2"
#   done  
  done

# ######## Sailing domain problems # ########
# sizes=(20 40 80)
#   sizes=(20)
#    for size in ${sizes[@]}; do
#   let "goal = size - 1"
#   let "goal = size/2"
#  # LRTDP
#  echo "${size}-$goal|lrtdp"
#  ../testsolver.out --sailing-size=$size --sailing-goal=$goal \
#  --algorithm=lrtdp --n=1 --v=$verbosity --heuristic=$heur \
#  --heuristic=$heur
# #  
    #LAO
#    echo "********************************************************"
#    echo "${size}-$goal|lao"
#   ../testsolver.out --sailing-size=$size --sailing-goal=$goal \
#   --algorithm=lao --v=$verbosity --n=$nsims \
#        --heuristic=$heur
       
    #PRM
#   echo "********************************************************"
#   echo "${size}-$goal|prm"
#   ../testsolver.out --sailing-size=$size --sailing-goal=$goal \
#   --algorithm=prm --v=$verbosity --n=$nsims \
#        --heuristic=$heur

      #ACARM
#     echo "********************************************************"
#   echo "${size}-$goal|acarm"
#   ../testsolver.out --sailing-size=$size --sailing-goal=$goal \
#   --algorithm=acarm --v=$verbosity --n=$nsims \
#        --heuristic=$heur

        # DET
#       echo "********************************************************"
#     echo "${size}-$goal|DET"
#     ../testsolver.out --sailing-size=$size --sailing-goal=$goal \
#   --algorithm=det --v=$verbosity --n=$nsims \
#        --heuristic=$heur

      # ALLD
#      echo "********************************************************"
#     echo "${size}-$goal|alld"
#     ../testsolver.out --sailing-size=$size --sailing-goal=$goal \
#   --algorithm=alld --v=$verbosity --n=$nsims \
#        --heuristic=$heur


#  # FLARES
#  for horizon in `seq 0 1`; do
#    echo "${size}-$goal|flares(${horizon})"
#    ../testsolver.out --sailing-size=$size --sailing-goal=$goal \
#    --algorithm=flares --horizon=$horizon --n=$nsims --v=$verbosity \
#    --heuristic=$heur
#  done
#  
#  # FLARES-PROB
#  echo "${size}-$goal|flares-prob(0.25)"
#  ../testsolver.out --sailing-size=$size --sailing-goal=$goal \
#  --algorithm=flares --depth=0.25 --use-prob-depth --n=$nsims --v=$verbosity \
#  --heuristic=$heur
#    
#  # HDP(0)
#    echo "${size}-$goal|hdp(0)"
#    ../testsolver.out --sailing-size=$size --sailing-goal=$goal --algorithm=hdp \
#      --n=$nsims --v=$verbosity -i=0 --heuristic=$heur
#            
#  # HDP(0,0)
#    echo "${size}-$goal|hdp(0,0)"
#    ../testsolver.out --sailing-size=$size --sailing-goal=$goal --algorithm=hdp \
#      --n=$nsims --v=$verbosity -i=0 --j=0 --heuristic=$heur
#  
#  # Approximate SSiPP  
#  t=2
#  for i in `seq 0 1`; do
#    echo "${size}-$goal||ssipp(${t})"
#    ../testsolver.out --sailing-size=$size --sailing-goal=$goal --algorithm=ssipp \
#      --horizon=$t --n=$nsims --v=$verbosity \
#      --heuristic=$heur
#      let "t *= 2"
#  done
# done
