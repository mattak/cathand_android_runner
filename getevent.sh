#!/bin/sh

adb shell getevent -t | egrep --line-buffered '^\[' | perl -ne '$|=1; $_ =~ s|\[\s*(\d+\.\d+)\s*\]\s+\S+\s+|\1 |g; print $_' 
