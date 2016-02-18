#!/bin/python2
#simple script to get System and architecture as a string
import os
import platform

arch = platform.machine()
system = platform.system()
dist = (platform.linux_distribution(distname="unknown", full_distribution_name=0)[0])
s = system + '-' + arch + '/' + dist
print (s)

