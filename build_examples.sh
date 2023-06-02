#!/bin/bash


for example in "param" "info_telemetry" "actions" "missions"
do
    echo "###############################################"
    echo "Building $example example"
    echo "###############################################"
    cd ./examples/$example

    cmake -Bbuild -H.
    cd build && make && cd ..

    cd ../../
done

