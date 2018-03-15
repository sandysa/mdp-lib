#!/bin/bash

nsims=100
verbosity=1
heuristic=(zero hmin domainGUSSP)
# heuristic=(zero domainGUSSP)
# grids=(map5GUSSP map5GUSSP-dense map6GUSSP map6GUSSP-dense)
grids=(map5GUSSP map6GUSSP)
search=(corridor map1GUSSP map2GUSSP map3GUSSP)
# search=(map3GUSSP)
rocks=(corridor map1GUSSP map2GUSSP map3GUSSP)
# rocks=(map3GUSSP)
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
for srs in ${search[@]}; do
      #LAO
       echo "********************************************************"
        echo "${srs}|lao|zero"
        ../testGussp.out --searchrescue=../data/searchrescue/$srs.sr \
        --algorithm=lao --v=$verbosity --n=$nsims \
        --heuristic=zero --uniform-goal-dist=true

        #FLARES
         for horizon in `seq 0 1`; do
            for heur in ${heuristic[@]};do
                echo "********************************************************"
                echo "${srs}|flares(${horizon})|${heur}"
                ../testGussp.out --searchrescue=../data/searchrescue/$srs.sr \
                --algorithm=soft-flares --horizon=$horizon --n=$nsims --v=$verbosity \
                --heuristic=$heur --alpha=0 --labelf=step --min_time=-1 --max_time=-1 \
                --uniform-goal-dist=true --dist=depth
            done
	done
done

######## Rock Sample domain problems # ########
# for rs in ${rocks[@]}; do
#     #LAO
#        echo "********************************************************"
#        echo "${rs}|lao|zero"
#        ../testGussp.out --rocksample=../data/rocksample/$rs.rs \
#        --algorithm=lao --v=$verbosity --n=$nsims \
#       --heuristic=zero --uniform-goal-dist=true
# 
#     #FLARES
#    for horizon in `seq 0 1`; do
#       for heur in ${heuristic[@]};do
#         echo "********************************************************"
#         echo "${rs}|flares(${horizon})|${heur}"
#         ../testGussp.out --rocksample=../data/rocksample/$rs.rs \
#         --algorithm=soft-flares --horizon=$horizon --n=$nsims --v=$verbosity \
#         --heuristic=$heur --alpha=0 --labelf=step --min_time=-1 --max_time=-1 --uniform-goal-dist=true
#     done
#   done
# done