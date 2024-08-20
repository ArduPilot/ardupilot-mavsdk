//
// Simple example to demonstrate how to do missions for APM using MAVSDK.
//

#include <chrono>
#include <cstdint>
#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/action/action.h>
#include <mavsdk/plugins/telemetry/telemetry.h>
#include <mavsdk/plugins/param/param.h>
#include <mavsdk/plugins/mission_raw/mission_raw.h>

#include <iostream>
#include <future>
#include <memory>
#include <thread>

using namespace mavsdk;
using std::chrono::seconds;
using std::this_thread::sleep_for;

void usage(const std::string& bin_name);
MissionRaw::MissionItem create_mission_item(uint32_t _seq, uint32_t _frame, uint32_t _command, uint32_t _current, uint32_t _autocontinue,
                                            float _param1, float _param2, float _param3, float _param4, 
                                            float _x, float _y, float _z, uint32_t _mission_type);

std::vector<MissionRaw::MissionItem> create_mission_raw();

void test_mission_raw(mavsdk::MissionRaw& mission_raw, mavsdk::Action& action, mavsdk::Telemetry& telemetry);

int main(int argc, char** argv)
{
    if (argc != 2) {
        usage(argv[0]);
        return 1;
    }

    Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};
    ConnectionResult connection_result = mavsdk.add_any_connection(argv[1]);

    if (connection_result != ConnectionResult::Success) {
        std::cerr << "Connection failed: " << connection_result << '\n';
        return 1;
    }

    auto system = mavsdk.first_autopilot(3.0);
    if (!system) {
        std::cerr << "Timed out waiting for system\n";
        return 1;
    }

    // Instantiate plugins.
    auto telemetry = Telemetry{system.value()};
    auto action = Action{system.value()};
    auto mission_raw = MissionRaw{system.value()};
    
    // while (telemetry.health_all_ok() != true) {
    //     std::cout << "Vehicle is getting ready to arm\n";
    //     std::cout << "Health: " << telemetry.health();
    //     sleep_for(seconds(5));
    // }
    test_mission_raw(mission_raw, action, telemetry);

    return 0;
}

void test_mission_raw(mavsdk::MissionRaw& mission_raw, mavsdk::Action& action, mavsdk::Telemetry& telemetry)
{
    std::vector<MissionRaw::MissionItem> mission_plan = create_mission_raw();

    // Upload Mission
    const MissionRaw::Result _uploadResult = mission_raw.upload_mission(mission_plan);
    if (_uploadResult != MissionRaw::Result::Success) {
        std::cerr << "Upload Mission failed: " << _uploadResult << '\n';
        return;
    }
    else {
        std::cout << "Upload Mission Succeed." << "\n";
    }

    // Arming the Vehicle.
    const Action::Result arm_result = action.arm();
    if (arm_result != Action::Result::Success) {
        std::cout << "Arming Result: " << arm_result << '\n';
        return;
    }


    // Mission Start
    std::cout << "Starting Mission \n ";
    const MissionRaw::Result _startResult = mission_raw.start_mission();
    if (_startResult != MissionRaw::Result::Success) {
        std::cerr << "Starting mission failed: " << _startResult << '\n';
        return;
    }
    else {
        std::cout << "Start Mission Succeeded." << "\n";
    }

    // Subscribe to mission progress.
    auto prom = std::promise<void>();
    auto fut = prom.get_future();

    MissionRaw::MissionProgressHandle handle =
        mission_raw.subscribe_mission_progress([&](MissionRaw::MissionProgress progress) {
            std::cout << "Progress: " << progress.current << "/" << progress.total;
            if (progress.current == progress.total) {
                mission_raw.unsubscribe_mission_progress(handle);
                prom.set_value();
            }
        });

     telemetry.subscribe_position([](Telemetry::Position position) {
        std::cout << "Altitude: " << position.relative_altitude_m << " m\n";
    });

    fut.get();

    std::cout << "Mission Finished: \n";
}

std::vector<MissionRaw::MissionItem> create_mission_raw()
{
    std::vector<MissionRaw::MissionItem> mission_raw_items;

    // _seq, _frame, _command, _current, _autocontinue, _param1, _param2, _param3, _param4, _x,  _y,  _z,  _mission_type
    // Add Home Position
    mission_raw_items.push_back(create_mission_item(0, 0, 16, 1, 1, 0, 0, 0, 0, 47.397742, 8.545594, 488, 0));

    // Add Mission Item 2-3
    mission_raw_items.push_back(create_mission_item(1, 3, 16, 0, 1, 0, 0, 0, 0, 47.39776911820642, 8.545794816614517, 30, 0));
    mission_raw_items.push_back(create_mission_item(2, 3, 16, 0, 1, 0, 0, 0, 0, 47.39814478901126, 8.544659618054993, 30, 0));

    // Return to Launch
    mission_raw_items.push_back(create_mission_item(3, 3, 20, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0));
    return mission_raw_items;
}

MissionRaw::MissionItem create_mission_item(uint32_t _seq, uint32_t _frame, uint32_t _command, uint32_t _current, uint32_t _autocontinue,
                                            float _param1, float _param2, float _param3, float _param4, 
                                            float _x, float _y, float _z, 
                                            uint32_t _mission_type)
{
    MissionRaw::MissionItem new_raw_item_nav{};
    new_raw_item_nav.seq = _seq;
    new_raw_item_nav.frame = _frame; // MAV_FRAME_GLOBAL_RELATIVE_ALT_INT 
    new_raw_item_nav.command = _command; // MAV_CMD_NAV_WAYPOINT or TAKEOFF or LAND etc.
    new_raw_item_nav.current = _current;
    new_raw_item_nav.autocontinue = _autocontinue;
    new_raw_item_nav.param1 = _param1; // Hold
    new_raw_item_nav.param2 = _param2; // Accept Radius
    new_raw_item_nav.param3 = _param3; // Pass Radius
    new_raw_item_nav.param4 = _param4; // Yaw
    new_raw_item_nav.x = int32_t(std::round(_x * 1e7));
    new_raw_item_nav.y = int32_t(std::round(_y * 1e7));
    new_raw_item_nav.z = _z;
    new_raw_item_nav.mission_type = 0;
    return new_raw_item_nav;
}

void usage(const std::string& bin_name)
{
    std::cerr << "Usage : " << bin_name << " <connection_url>\n"
              << "Connection URL format should be :\n"
              << " For TCP : tcp://[server_host][:server_port]\n"
              << " For UDP : udp://[bind_host][:bind_port]\n"
              << " For Serial : serial:///path/to/serial/dev[:baudrate]\n"
              << "For example, to connect to the simulator use URL: udp://:14540\n";
}

