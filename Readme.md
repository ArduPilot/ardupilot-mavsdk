
PX4_CUSTOM_MAIN_MODE_MANUAL
PX4_CUSTOM_MAIN_MODE_ALTCTL
PX4_CUSTOM_MAIN_MODE_POSCTL
PX4_CUSTOM_MAIN_MODE_AUTO
PX4_CUSTOM_MAIN_MODE_ACRO
PX4_CUSTOM_MAIN_MODE_OFFBOARD
PX4_CUSTOM_MAIN_MODE_STABILIZED
PX4_CUSTOM_MAIN_MODE_RATTITUDE

# Actions

1. Arm
Existing:
MavlinkCommandSender::CommandLong command{};

command.command = MAV_CMD_COMPONENT_ARM_DISARM;
command.params.maybe_param1 = 1.0f; // arm
command.target_component_id = _parent->get_autopilot_id();

if (_parent->get_flight_mode() == SystemImpl::FlightMode::Mission ||
        _parent->get_flight_mode() == SystemImpl::FlightMode::ReturnToLaunch) {
        _parent->set_flight_mode_async(
            SystemImpl::FlightMode::Hold


For Ardupilot, the following applies:
You can only arm or disarm in Stabilize, ACRO, AltHold, Loiter, and PosHold modes. You cannot arm your copter in AUTO mode. The AUTO_OPTIONS parameter can be used to alter this behaviour, allowing arming while in AUTO mode, and/or, allowing a mission takeoff command to start upon AUTO mode entry, even if the throttle has not been raised.

Stabilize, ACRO, AltHold, Loiter, and PosHold

if (_parent->autopilot() == SystemImpl::Autopilot::ArduPilot) {
        
        if (_parent->get_flight_mode() != SystemImpl::FlightMode::Offboard) {
            _parent->set_flight_mode_async(
                SystemImpl::FlightMode::Offboard,
                [callback, send_arm_command](MavlinkCommandSender::Result result, float) {
                    Action::Result action_result = action_result_from_command_result(result);
                    if (action_result != Action::Result::Success) {
                        if (callback) {
                            callback(action_result);
                        }
                    }
                    send_arm_command();
                });
            return;
        }
        send_arm_command();
    }


2. Disarming 
if disarming_allowed()

MavlinkCommandSender::CommandLong command{};

command.command = MAV_CMD_COMPONENT_ARM_DISARM;
command.params.maybe_param1 = 0.0f; // disarm
command.target_component_id = _parent->get_autopilot_id();

Ardupilot: Check for disarming conditions:

Stabilize, ACRO, AltHold, Loiter, or PosHold

If not change mode and disarm


10. GoToLocation
auto send_do_reposition =
        [this, callback, yaw_deg, latitude_deg, longitude_deg, altitude_amsl_m]() {
            MavlinkCommandSender::CommandInt command{};

            command.command = MAV_CMD_DO_REPOSITION;
            command.target_component_id = _parent->get_autopilot_id();
            command.params.maybe_param4 = static_cast<float>(to_rad_from_deg(yaw_deg));
            command.params.x = int32_t(std::round(latitude_deg * 1e7));
            command.params.y = int32_t(std::round(longitude_deg * 1e7));
            command.params.maybe_z = altitude_amsl_m;

            _parent->send_command_async(
                command, [this, callback](MavlinkCommandSender::Result result, float) {
                    command_result_callback(result, callback);
                });
        };

    // Change to Hold mode first
    if (_parent->get_flight_mode() != SystemImpl::FlightMode::Hold) {
        _parent->set_flight_mode_async(
            SystemImpl::FlightMode::Hold,
            [this, callback, send_do_reposition](MavlinkCommandSender::Result result, float) {
                Action::Result action_result = action_result_from_command_result(result);
                if (action_result != Action::Result::Success) {
                    command_result_callback(result, callback);
                    return;
                }
                send_do_reposition();
            });
        return;
    }

    send_do_reposition();


11. DoOrbit
MavlinkCommandSender::CommandInt command{};

command.command = MAV_CMD_DO_ORBIT;
command.target_component_id = _parent->get_autopilot_id();
command.params.maybe_param1 = radius_m;
command.params.maybe_param2 = velocity_ms;
command.params.maybe_param3 = static_cast<float>(yaw_behavior);
command.params.x = int32_t(std::round(latitude_deg * 1e7));
command.params.y = int32_t(std::round(longitude_deg * 1e7));
command.params.maybe_z = static_cast<float>(absolute_altitude_m);
Actions

12. Hold
 _parent->set_flight_mode_async(
        SystemImpl::FlightMode::Hold, [this, callback](MavlinkCommandSender::Result result, float) {
            command_result_callback(result, callback);
        });


13. Helper Function

Action::Result ActionImpl::taking_off_allowed() const
{
    if (!_in_air_state_known) {
        return Action::Result::CommandDeniedLandedStateUnknown;
    }

    if (_in_air) {
        return Action::Result::CommandDeniedNotLanded;
    }

    return Action::Result::Success;
}

Action::Result ActionImpl::disarming_allowed() const
{
    if (!_in_air_state_known) {
        return Action::Result::CommandDeniedLandedStateUnknown;
    }

    if (_in_air) {
        return Action::Result::CommandDeniedNotLanded;
    }

    return Action::Result::Success;
}

Setting and Getting Flight Modes for APM


3. Terminate
MavlinkCommandSender::CommandLong command{};

command.command = MAV_CMD_DO_FLIGHTTERMINATION;
command.params.maybe_param1 = 1.0f;
command.target_component_id = _parent->get_autopilot_id();

4. Kill
MavlinkCommandSender::CommandLong command{};

command.command = MAV_CMD_COMPONENT_ARM_DISARM;
command.params.maybe_param1 = 0.0f; // kill
command.params.maybe_param2 = 21196.f; // magic number to enforce in-air
command.target_component_id = _parent->get_autopilot_id();

5. Reboot 
MavlinkCommandSender::CommandLong command{};

command.command = MAV_CMD_PREFLIGHT_REBOOT_SHUTDOWN;
command.params.maybe_param1 = 1.0f; // reboot autopilot
command.params.maybe_param2 = 1.0f; // reboot onboard computer
command.params.maybe_param3 = 1.0f; // reboot camera
command.params.maybe_param4 = 1.0f; // reboot gimbal
command.target_component_id = _parent->get_autopilot_id();

6. Shutdown
MavlinkCommandSender::CommandLong command{};

command.command = MAV_CMD_PREFLIGHT_REBOOT_SHUTDOWN;
command.params.maybe_param1 = 2.0f; // shutdown autopilot
command.params.maybe_param2 = 2.0f; // shutdown onboard computer
command.params.maybe_param3 = 2.0f; // shutdown camera
command.params.maybe_param4 = 2.0f; // shutdown gimbal
command.target_component_id = _parent->get_autopilot_id();

7. Takeoff
MavlinkCommandSender::CommandLong command{};

command.command = MAV_CMD_NAV_TAKEOFF;
command.target_component_id = _parent->get_autopilot_id();

if (_parent->autopilot() == SystemImpl::Autopilot::ArduPilot) {
    command.params.maybe_param7 = get_takeoff_altitude().second;
}

8. Land
MavlinkCommandSender::CommandLong command{};

command.command = MAV_CMD_NAV_LAND;
command.params.maybe_param4 = NAN; // Don't change yaw.
command.target_component_id = _parent->get_autopilot_id();

9. RTL
_parent->set_flight_mode_async(
        SystemImpl::FlightMode::ReturnToLaunch,
        [this, callback](MavlinkCommandSender::Result result, float) {
            command_result_callback(result, callback);
        });


Arm         - Ardupilot can only do in some flight modes, pre flight checks, anything else?
Disarm      - can do in any modes but prefer to do in loiter mode?
Takeoff     - can only do in certain modes
Land     
ReturnToLaunch
GotoLocation  - PX4 (lat, long, abs_alt, yaw)
Hold

GetTakeoffAltitude
SetTakeoffAltitude
GetMaximumSpeed
SetMaximumSpeed
GetReturnToLaunchAltitude
SetReturnToLaunchAltitude

DoOrbit (radius, velocity, orbitbehav, lat, long, abs_alt)


AUTO MODE for Copter: https://ardupilot.org/copter/docs/auto-mode.html#auto-mode

You must arm your copter before you can engage AUTO mode. See Arming the motors for details on how to arm your copter.


Starting a Mission in Auto Mode

AUTO_OPTIONS from https://ardupilot.org/copter/docs/parameters.html#auto-options
AUTO_OPTIONS: Auto mode options
Note: This parameter is for advanced users

A range of options that can be applied to change auto mode behaviour. Allow Arming allows the copter to be armed in Auto. Allow Takeoff Without Raising Throttle allows takeoff without the pilot having to raise the throttle. Ignore pilot yaw overrides the pilot’s yaw stick being used while in auto.
Bitmask
Bit 	Meaning
0 	Allow Arming
1 	Allow Takeoff Without Raising Throttle
2 	Ignore pilot yaw


At high level, the steps are: check that the vehicle is able to arm, set the mode to GUIDED, command the vehicle to arm, takeoff and block until we reach the desired altitude.

Tip

Copter is usually started in GUIDED mode.

    For Copter 3.2.1 and earlier you cannot take off in AUTO mode (if you need to run a mission you take off in GUIDED mode and then switch to AUTO mode once you’re in the air).
    Starting from Copter 3.3 you can takeoff in AUTO mode (provided the mission has a MAV_CMD_NAV_TAKEOFF command) but the mission will not start until you explicitly send the MAV_CMD_MISSION_START message.

By contrast, Plane apps take off using the MAV_CMD_NAV_TAKEOFF command in a mission. Plane should first arm and then change to AUTO mode to start the mission.

Missions should normally have an RTL as their final command to ensure the copter will return after the mission completes. Alternatively the final command could be a LAND with a different location. 

Missions:
1st Element is the Home position
2nd Element always Takeoff
3rd to n-1th element any nav commands
nth element should be RTL



# General Notes about Integration Tests:


## Hover related tests
The following tests in the action_hover_sync.cpp with fail 

SitlTest.PX4ActionHoverSyncDefault
SitlTest.PX4ActionHoverSyncHigher
SitlTest.PX4ActionHoverSyncLower

with the error:
Value of: poll_condition_with_timeout( [telemetry]() { return telemetry->flight_mode() == Telemetry::FlightMode::Takeoff; }, std::chrono::seconds(10))
Value of: poll_condition_with_timeout( [telemetry]() { return telemetry->flight_mode() == Telemetry::FlightMode::Hold; }, std::chrono::seconds(20))

The root problem is that there is no flight mode takeoff in APM at the moment. In PX4 as soon as the autopilot gets a takeoff command it changes the flight mode to Takeoff which is also conveyed by the Telemetry::FlightMode::Takeoff. 

The second problem is that the APM autopilot is armed under specific modes (e.g. Guided) and even though the autopilot has successfully taken off and loitering at the specified altitude, the flight mode will still remain Guided and not Hold. In PX4 it automatically changes to Hold once reached the target altitude.

## VTOL related tests
will also fail since its not verified yet.

## Action Goto
will also fail, command not working yet. Command is being sent but AP rejects the command.

## Action Hold (PX4ActionHold)
will also fail because there is a delay of 1 second after arming which leads to disarming of the vehicle since the SITL is running with a SPEEDUP of 10.
[09:17:27|Debug] MAVLink: info: Arming motors (system_impl.cpp:239)
[09:17:28|Debug] MAVLink: info: Disarming motors (system_impl.cpp:239)

Action::Result action_ret = action->arm();
EXPECT_EQ(action_ret, Action::Result::Success);
std::this_thread::sleep_for(std::chrono::seconds(1));

## FollowMe related tests will also fail

MAVLINK_MSG_ID_ACTUATOR_CONTROL_TARGET 140 fails for APM


/__w/MAVSDK/MAVSDK/src/integration_tests/telemetry_sync.cpp:29: Failure
Expected equality of these values:
  telemetry->set_rate_actuator_control_target(10.0)
    Which is: Command Denied
  Telemetry::Result::Success
    Which is: Success
[09:24:52|Debug] MAVLink: info: No ap_message for mavlink id (140) (system_impl.cpp:239)


HIL_STATE_QUATERNION 115 fails for APM

/__w/MAVSDK/MAVSDK/src/integration_tests/telemetry_sync.cpp:31: Failure
Expected equality of these values:
  telemetry->set_rate_ground_truth(10.0)
    Which is: Command Denied
  Telemetry::Result::Success
    Which is: Success
[09:24:52|Debug] MAVLink: info: No ap_message for mavlink id (115) (system_impl.cpp:239)



Mavlink passthrough fails because of Command fail MAV_CMD_REQUEST_AUTOPILOT_CAPABILITIES 520