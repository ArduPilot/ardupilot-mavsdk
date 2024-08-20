#!/usr/bin/env bash

set -e

DOCKER_CMD=docker

if ! command -v $DOCKER_CMD &> /dev/null
then
    echo "docker not found, trying podman instead"
    DOCKER_CMD=podman
fi

$DOCKER_CMD build -f Dockerfile-Ubuntu-22.04 -t khedar/mavsdk-ubuntu-22.04 .
$DOCKER_CMD build -f Dockerfile-Ubuntu-22.04-APM-SITL-Copter-4.5.5 -t khedar/mavsdk-ubuntu-22.04-apm-sitl-copter-4.5.5 .
$DOCKER_CMD build -f Dockerfile-Ubuntu-22.04-APM-SITL-Rover-4.5.5 -t khedar/mavsdk-ubuntu-22.04-apm-sitl-rover-4.5.5 .
