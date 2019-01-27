#!/bin/sh

(sleep 1 && ivi-shell -f "HUDTDS" > /dev/null ) &
(sleep 1 && ivi-shell -z "HUDTDS":10000 > /dev/null & ) &
(sleep 1 && ivi-shell -a "HUDTDS":255 > /dev/null & ) &

./hudtds
