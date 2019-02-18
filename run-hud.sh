#!/bin/sh


(sleep 3 && ivi-shell -f "HUDTDS" 2>&1 > /dev/null ) > /dev/null &
# (sleep 1 && ivi-shell -z "HUDTDS":10000 2>&1 > /dev/null ) > /dev/null &
# (sleep 1 && ivi-shell -a "HUDTDS":255 2>&1 > /dev/null ) > /dev/null &
#
# WAYLAND_DEBUG=1 ./hudtds
./hudtds
