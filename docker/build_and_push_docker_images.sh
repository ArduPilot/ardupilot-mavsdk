#!/usr/bin/env bash

set -e

DOCKER_CMD=docker

if ! command -v $DOCKER_CMD &> /dev/null
then
    echo "docker not found, trying podman instead"
    DOCKER_CMD=podman
fi

$DOCKER_CMD build -f Dockerfile-Ubuntu-20.04 -t khedar/mavsdk-ubuntu-20.04 .
$DOCKER_CMD build -f Dockerfile-Ubuntu-20.04-APM-SITL-Copter-4.1.2 -t khedar/mavsdk-ubuntu-20.04-apm-sitl-copter-4.1.2 .
$DOCKER_CMD build -f Dockerfile-Ubuntu-20.04-APM-SITL-Rover-4.1.2 -t khedar/mavsdk-ubuntu-20.04-apm-sitl-rover-4.1.2 .

$DOCKER_CMD push khedar/mavsdk-ubuntu-20.04:latest
$DOCKER_CMD push khedar/mavsdk-ubuntu-20.04-apm-sitl-copter-4.1.2
$DOCKER_CMD push khedar/mavsdk-ubuntu-20.04-apm-sitl-rover-4.1.2
