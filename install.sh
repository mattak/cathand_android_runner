#!/bin/sh

ARCH=$(adb shell getprop ro.product.cpu.abi | perl -pne 's|\r||g')
SRC_FILE=./app/build/intermediates/cmake/debug/obj/$ARCH/cathand.so
OUT_FILE=/data/local/tmp/cathand
adb push $SRC_FILE $OUT_FILE
adb shell chmod +x $OUT_FILE
echo adb push /tmp/getevent /data/local/tmp/getevent.tsv
echo adb shell /data/local/tmp/cathand /dev/input/event2 /data/local/tmp/getevent.tsv
