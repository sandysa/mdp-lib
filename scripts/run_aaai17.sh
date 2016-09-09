#!/bin/bash

nsims=100
verbosity=0
heur=zero

tracks=(known/ring-3-error known/ring-4-error)
        
# ######## Racetrack domain problems # ########
for track in ${tracks[@]}; do
#   echo "${track}|lrtdp"
#   ../testsolver.out --track=../data/tracks/$track.track \
#   --algorithm=lrtdp --v=$verbosity --n=$nsims \
#   --no-initial-plan --heuristic=$heur
  
  # FLARES
  for horizon in `seq 0 1`; do
    echo "${track}|flares(${horizon})"
      ../testsolver.out --track=../data/tracks/$track.track \
      --algorithm=flares --horizon=$horizon --v=$verbosity --n=$nsims \
      --heuristic=$heur
  done
       
  # HDP(0)
  echo "${track}|hdp(0)"
  ../testsolver.out --track=../data/tracks/$track.track \
  --algorithm=hdp --i=0 --v=$verbosity --n=$nsims \
  --heuristic=$heur
  
  # HDP(0)
  echo "${track}|hdp(0,0)"
  ../testsolver.out --track=../data/tracks/$track.track \
  --algorithm=hdp --i=0 --j=0 --v=$verbosity --n=$nsims \
  --heuristic=$heur
  
  # SSiPP(t)
  t=2
  for i in `seq 0 1`; do
    echo "${track}|ssipp(${t})"
      ../testsolver.out --track=../data/tracks/$track.track \
      --algorithm=ssipp --horizon=$t --v=$verbosity --n=$nsims \
      --heuristic=$heur
    let "t *= 2"
  done  
done

# ######## Sailing domain problems # ########
sizes=(10 20)
for size in ${sizes[@]}; do
  let "goal = size - 1"
  # LRTDP
  echo "${size}-$goal|lrtdp"
  ../testsolver.out --sailing-size=$size --sailing-goal=$goal --algorithm=lrtdp \
  --n=$nsims --v=$verbosity --heuristic=$heur --no-initial-plan
  
  # FLARES
  for horizon in `seq 0 1`; do
    echo "${size}-$goal|flares(${horizon})"
    ../testsolver.out --sailing-size=$size --sailing-goal=$goal --algorithm=flares \
    --horizon=$horizon --n=$nsims --v=$verbosity --heuristic=$heur
  done
    
  # HDP(0)
    echo "${size}-$goal|hdp(0)"
    ../testsolver.out --sailing-size=$size --sailing-goal=$goal --algorithm=hdp \
      --n=$nsims --v=$verbosity -i=0 --heuristic=$heur
            
  # HDP(0)
    echo "${size}-$goal|hdp(0,0)"
    ../testsolver.out --sailing-size=$size --sailing-goal=$goal --algorithm=hdp \
      --n=$nsims --v=$verbosity -i=0 --j=0 --heuristic=$heur
  
  # Approximate SSiPP  
  t=2
  for i in `seq 0 1`; do
    echo "${size}-$goal||ssipp(${t})"
    ../testsolver.out --sailing-size=$size --sailing-goal=$goal --algorithm=ssipp \
      --horizon=$t --n=$nsims --v=$verbosity \
      --heuristic=$heur
      let "t *= 2"
  done
done