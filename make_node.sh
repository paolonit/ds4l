#!/bin/sh

IFS=' '
str="$(cat /proc/devices | grep DS4)"
var=$(echo "$str" | cut -d" " -f1)
mknod /dev/ds4-device c $var 0
