#!/bin/bash

nsims=100
verbosity=1
heuristic=(zero hmin-solve-all domainGUSSP)
rew=1
endrange=(5 6)
detChoice=(0 1 2)
# endrange=(14)

while IFS='' read -r line || [[ -n "$line" ]]; do
    a=( $line )
#     echo "***********************************************"
#     echo $line
#     echo "reward="$rew
#     for horizon in `seq 0 1`; do
#      echo "******ALG = flares|(${horizon})"
#      ../testsolver.out --start-soc=${a[0]} --end-soc=${a[1]} --start-time=${a[2]} --end-time=${a[3]} \
#      --reward=$rew --algorithm=flares --horizon=$horizon --n=$nsims --v=$verbosity --heuristic=hmin-solve-all
#        done
#         echo "******ALG = LAO "
#         ../testsolver.out --start-soc=${a[0]} --end-soc=${a[1]} --start-time=${a[2]} --end-time=${a[3]} \
#         --reward=$rew --algorithm=lao --n=$nsims --v=$verbosity --heuristic=$heur
# #     
#         echo "******ALG = DET "
#         ../testsolver.out --start-soc=${a[0]} --end-soc=${a[1]} --start-time=${a[2]} --end-time=${a[3]} \
#         --reward=$rew --algorithm=det --n=$nsims --v=$verbosity --heuristic=$heur
#     
#         echo "******ALG = prm "
#         ../testsolver.out --start-soc=${a[0]} --end-soc=${a[1]} --start-time=${a[2]} --end-time=${a[3]} \
#         --reward=$rew --algorithm=prm --n=$nsims --v=$verbosity --heuristic=$heur

#         echo "******ALG = ALLD "
#         ../testsolver.out --start-soc=${a[0]} --end-soc=${a[1]} --start-time=${a[2]} --end-time=${a[3]} \
#         --reward=$rew --algorithm=alld --n=$nsims --v=$verbosity --heuristic=$heur
# #      
#         echo "******ALG = ACARM "
#         ../testsolver.out --start-soc=${a[0]} --end-soc=${a[1]} --start-time=${a[2]} --end-time=${a[3]} \
#         --reward=$rew --algorithm=acarm --n=$nsims --v=$verbosity --heuristic=$heur
       
    
##ALTERED FOR GUSSP 
    #LAO
   for ex in ${endrange[@]};do
        echo "********************************************************"
        echo "lao|zero|$ex"
        ../testGussp.out --start-soc=${a[0]} --end-soc=${a[1]} --start-time=${a[2]} --exit-range=$endrange --reward=$rew \
        --algorithm=lao --v=$verbosity --n=$nsims \
       --heuristic=zero --uniform-goal-dist=true

        # FLARES
       for horizon in `seq 0 1`; do
         for heur in ${heuristic[@]};do
                echo "********************************************************"
                echo "flares(${horizon})|${heur}|$ex"
                ../testGussp.out --start-soc=${a[0]} --end-soc=${a[1]} --start-time=${a[2]} --exit-range=$ex --reward=$rew \
                --algorithm=soft-flares --horizon=$horizon --n=$nsims --v=$verbosity \
                --heuristic=$heur --alpha=0 --labelf=step --min_time=-1 --max_time=-1  --uniform-goal-dist=true
             done
        done
        
               # DETGUSSP
       for dchoice in ${detChoice[@]};do
         for heur in ${heuristic[@]};do
                echo "********************************************************"
                echo "detGUSSP(${dchoice})|${heur}|$ex"
                ../testGussp.out --start-soc=${a[0]} --end-soc=${a[1]} --start-time=${a[2]} --exit-range=$ex --reward=$rew \
                --algorithm=detGUSSP --n=$nsims --v=$verbosity \
                --heuristic=$heur --uniform-goal-dist=true --det_choice=$dchoice
             done
        done
      done
done < "$1"


#     echo "${track}|"
#       ../testsolver.out --track=../data/tracks/$track.track \
#       --algorithm=flares --horizon=$horizon --v=$verbosity --n=$nsims \
#       --heuristic=$heur