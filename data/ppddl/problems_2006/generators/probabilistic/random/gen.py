#! /usr/bin/env python

# generation of random domain and problem
# use slightly modified FF  for
# 1. random walk
# 2. testing solvability (this is yet to be done)
# 

# basically genreset is almost enougth
# to make sure, we are using FF to test the
# vailidity and solvability

# various parameters are involved
# -i number of initial facts
# -a number of prob actions
# -s number of predicates
# -r random walk length
# -t max predicate arity
# -c max action arity
# -p number of effects in the probability construct
# -e number of effects
# -d number of precondition
# -n probability of negating preconditions
# -m probability of negating effects
# -h probability of cutoff random walk path probability

import os, sys, re, string

def main():

    found = 0
    while(found == 0):
        os.system("rm -f ttt validproblem")
        os.system("genreset -i 20 -a 5 -s 5 -d 3 -e 5 -h 0.05 -n 0 -p 6 -r 100 -m 50;ulimit -St 10;  ff -o det_domain -f b-problem -r ttt");

        #check valid problem
        if os.path.exists("validproblem"):
            #if os.path.exists("ttt"):
            found = 1
    
main()
os.system("rm -f ttt action_table a-problem validproblem b-problem det_domain prob_domain prob_domain_reset")
os.system("mv b-problem_reset random-problem-reset")
