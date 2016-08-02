#! /usr/bin/env python

import os, sys, re, string

if len(sys.argv) < 2:
    print "usage make.py genreset or make.py genreverse"
    sys.exit(1)
    
os.system("make " + sys.argv[1])
os.system("cd FF-mod;make;cp ff ../ffmod;cd ..")
os.system("cd FF-v2.3;make;cp ff ../.;cd ..")
