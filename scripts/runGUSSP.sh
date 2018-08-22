#!/bin/bash

nsims=100
verbosity=1
heuristic=(zero hmin domainGUSSP)
# grids=(map5GUSSP map5GUSSP-dense map6GUSSP map6GUSSP-dense)
search=(map1GUSSP map2GUSSP map3GUSSP map3GUSSP-dense)
# rocks=(map2GUSSP map3GUSSP map1GUSSP-dense)
rocks=(map2GUSSP-dense)
detChoice=(0 1 2)
detheuristic=(zero domainGUSSP)
######## Gridworld domain problems # ########
# for gw in ${grids[@]}; do
#     #LAO
#         echo "********************************************************"
#         echo "${gw}|lao|zero"
#         ../testGussp.out --grid=../data/gws/$gw.gw \
#         --algorithm=lao --v=$verbosity --n=$nsims \
#         --heuristic=zero --uniform-goal-dist=true
# 
#         # FLARES
#       for horizon in `seq 0 1`; do
#           for heur in ${heuristic[@]};do
#             echo "********************************************************"
#             echo "${gw}|flares(${horizon})|${heur}"
#             ../testGussp.out --grid=../data/gws/$gw.gw \
#             --algorithm=soft-flares --horizon=$horizon --n=$nsims --v=$verbosity \
#             --heuristic=$heur --alpha=0 --labelf=step --dist=depth --min_time=-1 \
#             --max_time=-1 --uniform-goal-dist=true
#         done
# 	done
# done

 ######## Search and Rescue domain problems # ########
# for srs in ${search[@]}; do
#       #LAO
#        echo "********************************************************"
#         echo "${srs}|lao|zero"
#         ../testGussp.out --searchrescue=../data/searchrescue/$srs.sr \
#         --algorithm=lao --v=$verbosity --n=$nsims \
#         --heuristic=zero --uniform-goal-dist=true
# 
#         #FLARES
#          for horizon in `seq 0 1`; do
#             for heur in ${heuristic[@]};do
#                 echo "********************************************************"
#                 echo "${srs}|flares(${horizon})|${heur}"
#                 ../testGussp.out --searchrescue=../data/searchrescue/$srs.sr \
#                 --algorithm=soft-flares --horizon=$horizon --n=$nsims --v=$verbosity \
#                 --heuristic=$heur --alpha=0 --labelf=step --min_time=-1 --max_time=-1 \
#                 --uniform-goal-dist=true --dist=depth
#             done
#         done
#         
#                  # DETGUSSP
#        for dchoice in ${detChoice[@]};do
#          for heur in ${heuristic[@]};do
#                 echo "********************************************************"
#                 echo "${srs}|detGUSSP(${dchoice})|${heur}"
#                 ../testGussp.out --searchrescue=../data/searchrescue/$srs.sr \
#                 --algorithm=detGUSSP --n=$nsims --v=$verbosity \
#                 --heuristic=$heur --uniform-goal-dist=true --det_choice=$dchoice
#              done
#         done
# done

######## Rock Sample domain problems # ########
for rs in ${rocks[@]}; do
    #LAO
       echo "********************************************************"
       echo "${rs}|lao|zero"
       ../testGussp.out --rocksample=../data/rocksample/$rs.rs \
       --algorithm=lao --v=$verbosity --n=$nsims \
      --heuristic=zero --goal-dist=uniform

    #FLARES
 
     horizon=1
#    for horizon in `seq 0 1`; do
      for heur in ${heuristic[@]};do
        echo "********************************************************"
        echo "${rs}|flares(${horizon})|${heur}"
        ../testGussp.out --rocksample=../data/rocksample/$rs.rs \
        --algorithm=soft-flares --horizon=$horizon --n=$nsims --v=$verbosity \
        --heuristic=$heur --alpha=0 --labelf=step --min_time=-1 --max_time=-1 --goal-dist=uniform
    done
#   done
 
             # DETGUSSP
       for dchoice in ${detChoice[@]};do
         for heur in ${detheuristic[@]};do
                echo "********************************************************"
                echo "${rs}|detGUSSP(${dchoice})|${heur}"
                ../testGussp.out --rocksample=../data/rocksample/$rs.rs \
                --algorithm=detGUSSP --n=$nsims --v=$verbosity \
                --heuristic=$heur --goal-dist=uniform --det_choice=$dchoice
             done
        done
done