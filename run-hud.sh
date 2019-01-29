#!/bin/sh

(sleep 1 && ivi-shell -f "HUDTDS" ) &
(sleep 1 && ivi-shell -z "HUDTDS":10000 2>&1 > /dev/null ) > /dev/null &
(sleep 1 && ivi-shell -a "HUDTDS":255 2>&1 > /dev/null ) > /dev/null &

./hudtds
