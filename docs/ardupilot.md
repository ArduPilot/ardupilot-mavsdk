## Important ArduPilot Points
Copied from Various ArduPilot WiKi Documentation!
### Arm
For Ardupilot, the following applies:
You can only arm or disarm in Stabilize, ACRO, AltHold, Loiter, and PosHold modes. You cannot arm your copter in AUTO mode. The AUTO_OPTIONS parameter can be used to alter this behaviour, allowing arming while in AUTO mode, and/or, allowing a mission takeoff command to start upon AUTO mode entry, even if the throttle has not been raised.

### Takeoff
Need to set the take off altitude before sending takeoff command. 

### Mission
Missions should have the following form:
1st Element is the Home position
2nd Element always Takeoff (The mission item is ignored if the copter is already in air.)
3rd to n-1th element any nav commands
nth element should be RTL (This is optional)


Missions are executed either by the following ways:
1. Arm the vehicle in guided mode, takeoff to a certain altitude, change the mode to auto and the misssion should start running. The copter cannot be armed in Auto mode generally. See the next option for a workaround.
2. (Not Tested with MAVSDK) use AUTO_OPTIONS to allow takeoff in auto mode (https://ardupilot.org/copter/docs/parameters.html#auto-options)
   This should automatically start the mission from ground.

### Auto Mode

https://ardupilot.org/copter/docs/auto-mode.html#auto-mode

You must arm your copter before you can engage AUTO mode. See Arming the motors for details on how to arm your copter
Missions should normally have an RTL as their final command to ensure the copter will return after the mission completes. Alternatively the final command could be a LAND with a different location. 