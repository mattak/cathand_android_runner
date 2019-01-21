#!/bin/sh
# [  128893.610543] /dev/input/event2: 0000 0000 00000000

adb shell getevent -t | egrep --line-buffered '^\[' | perl -ne '$|=1; $_ =~ s|\[\s*(\d+\.\d+)\s*\]\s+\S+\s+|\1 |g; print $_' 

