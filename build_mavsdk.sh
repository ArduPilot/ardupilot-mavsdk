#!/bin/bash
cd ../MAVSDK

# Get 
git submodule update --init --recursive

# Build MAVLINK system wide
cmake -Bthird_party/mavlink/build -Sthird_party/mavlink
cmake --build third_party/mavlink/build

# Build MAVSDK
cmake -Bbuild/default -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=false -H.
cmake --build build/default -j8
sudo cmake --build build/default --target install

cd ../tools