#!/bin/bash


for example in "param" "info_telemetry" "actions" "missions_rover" "missions_copter"
do
    echo "###############################################"
    echo "Building $example example"
    echo "###############################################"
    cd ./examples/$example

    cmake -Bbuild -H.
    cd build && make && cd ..

    cd ../../
done

