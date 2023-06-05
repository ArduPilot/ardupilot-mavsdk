#!/bin/bash
pkill screen
pkill screen
pkill screen

APM_HOME_LAT=${PX4_HOME_LAT:-47.397742}
APM_HOME_LONG=${PX4_HOME_LON:-8.545594}
APM_HOME_ALT=${PX4_HOME_ALT:-488}
APM_HOME_DIR=180
echo "Starting the SITL"

cd /home/user/ArduPilot/

screen -dmS ardupilot /home/user/ArduPilot/build/sitl/bin/arducopter -S -w --model + \
                     --speedup 1 \
                     --defaults /home/user/ArduPilot/Tools/autotest/default_params/copter.parm \
                     -I0 \
                     --home ${APM_HOME_LAT},${APM_HOME_LONG},${APM_HOME_ALT},${APM_HOME_DIR}


sleep 5

