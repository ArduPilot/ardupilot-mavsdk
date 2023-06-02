# ardupilot-mavsdk

This repository provides basic tools and examples to run control ArduPilot SITL using MAVSDK.

- docker

This folder contains docker files to build a docker container which contains the dev environment for ArduPilot SITL as well as MAVSDK. 

- docs

This contains documentation on pecularities of various ArduPilot features needed to control a vehicle running this autopilot. This will grow over time. Best documentation is however the official Ardupilot documentation:

https://ardupilot.org/copter

- examples
This contains many examples on how to use MAVSDK library to control ArduPilot SITL.

The following sequence should be followed in order to setup everything:
1. Build the docker images in ```bash docker/ ``` folder using the script ```bash ./docker/build_and_push_docker_images.sh ```

2. Clone the MAVSDK library on your system. Checkout specfic tag or version if don't want to use the main brach.

3. Adapt the ```bash PATH_TO_MAVSDK``` and ```bash PATH_TO_TOOLS``` in the script ```bash run_docker.sh```

4. Run the script ```bash run_docker.sh```

5. In the container so launched, compile MAVSDK library by running the script ```bash build_mavsdk.sh```

6. Similarly build the examples by running the script ```bash build_examples.sh```

7. Launch the SITL by running ```bash run_sitl_apm.sh```

8. Run any of the examples by running ```bash run_example.sh```

## Questions 

For any questions, please raise issue in this repository.





