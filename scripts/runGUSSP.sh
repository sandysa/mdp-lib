#!/bin/bash

nsims=100
verbosity=0
heuristic=(hmin domainGUSSP)
grids=(map5GUSSP map5GUSSP-dense map6GUSSP map6GUSSP-dense)
search=(corridor map1GUSSP map2GUSSP map1GUSSP-dense map2GUSSP-dense)
rocks=(corridor map1GUSSP map2GUSSP map3GUSSP map1GUSSP-dense map2GUSSP-dense)
# ######## Gridworld domain problems # ########
# for gw in ${grids[@]}; do
# 	#LAO
#         echo "********************************************************"
#         echo "${gw}|lao"
#         ../testGussp.out --grid=../data/gws/$gw.gw \
#         --algorithm=lao --v=$verbosity --n=$nsims \
#        #--heuristic=$heur
# 
# 	# FLARES
# 	for horizon in `seq 0 1`; do
# 	echo "********************************************************"
# 	echo "${gw}|flares(${horizon})"
# 	../testGussp.out --grid=../data/gws/$gw.gw \
# 	--algorithm=flares --horizon=$horizon --n=$nsims --v=$verbosity \
# 	--heuristic=$heur
# 	done
# done

 ######## Search and Rescue domain problems # ########
for srs in ${search[@]}; do
      for heur in ${heuristic[@]};do
        #LAO
       echo "********************************************************"
        echo "${rs}|lao|${heur}"
        ../testGussp.out --searchrescue=../data/searchrescue/$srs.sr \
        --algorithm=lao --v=$verbosity --n=$nsims \
        --heuristic=$heur

        #FLARES
            for horizon in `seq 0 1`; do
                echo "********************************************************"
                echo "${srs}|flares(${horizon})|${heur}"
                ../testGussp.out --searchrescue=../data/searchrescue/$srs.sr \
                --algorithm=flares --horizon=$horizon --n=$nsims --v=$verbosity \
                --heuristic=$heur
            done
	done
done

######## Rock Sample domain problems # ########
# for rs in ${rocks[@]}; do
#     for heur in ${heuristic[@]};do
#   #LAO
#        echo "********************************************************"
#        echo "${rs}|lao|${heur}"
#        ../testGussp.out --rocksample=../data/rocksample/$rs.rs \
#        --algorithm=lao --v=$verbosity --n=$nsims \
#       --heuristic=$heur
# 
#     #FLARES
#     for horizon in `seq 0 1`; do
#         echo "********************************************************"
#         echo "${rs}|flares(${horizon})|${heur}"
#         ../testGussp.out --rocksample=../data/rocksample/$rs.rs \
#         --algorithm=flares --horizon=$horizon --n=$nsims --v=$verbosity \
#         --heuristic=$heur
#     done
#   done
# done