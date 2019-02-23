#!/bin/sh


pgrep gpds > /dev/null
if [ $? == 1 ]; then
    gpsd -b /dev/ttymxc2 -P /tmp/gps.pid -S 2947
fi


(sleep 3 && ivi-shell -f "HUDTDS" 2>&1 > /dev/null ) > /dev/null &
# (sleep 1 && ivi-shell -z "HUDTDS":10000 2>&1 > /dev/null ) > /dev/null &
# (sleep 1 && ivi-shell -a "HUDTDS":255 2>&1 > /dev/null ) > /dev/null &
#
# export WAYLAND_DEBUG=1
# WAYLAND_DEBUG=1 ./hudtds
./hudtds
