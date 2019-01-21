#!/bin/sh

ARCH=$(adb shell getprop ro.product.cpu.abi)
adb push ./app/build/intermediates/cmake/debug/obj/$ARCH/cathand.so /data/local/tmp/cathand
adb shell chmod +x /data/local/tmp/cathand
adb shell /data/local/tmp/cathand
