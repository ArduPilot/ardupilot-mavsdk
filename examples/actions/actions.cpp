//
// Simple example to demonstrate how takeoff and land using MAVSDK.
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

void test_takeoff_land( mavsdk::Telemetry& _aTelemetry,
                        mavsdk::Action& _aAction);

void test_takeoff_RTL( mavsdk::Telemetry& _aTelemetry,
                        mavsdk::Action& _aAction);

void test_takeoff_goto_land( mavsdk::Telemetry& _aTelemetry,
                        mavsdk::Action& _aAction);

void test_terminate( mavsdk::Telemetry& _aTelemetry,
                        mavsdk::Action& _aAction);

void test_reboot( mavsdk::Telemetry& _aTelemetry,
                        mavsdk::Action& _aAction);

void usage(const std::string& bin_name);

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

    test_takeoff_land(telemetry, action);
    //test_takeoff_RTL( telemetry, action);
    //test_terminate( telemetry, action);
    //test_reboot( telemetry, action);

    return 0;
}


void test_takeoff_goto_land( mavsdk::Telemetry& _aTelemetry,
                        mavsdk::Action& _aAction)
{
    // Check until vehicle is ready to arm
    // while (_aTelemetry.health_all_ok() != true) {
    //     std::cout << "Vehicle is getting ready to arm\n";
    //     std::cout << "Health: " << _aTelemetry.health();
    //     sleep_for(seconds(5));
    // }

    // Arm vehicle
    std::cout << "Arming...\n";
    const Action::Result arm_result = _aAction.arm();
    if (arm_result != Action::Result::Success) {
        std::cerr << "Arming failed: " << arm_result << '\n';
        return;
    }
    std::cout << "Armed.\n" ;
    
    std::cout << "Setting takeoff altitude\n";
    _aAction.set_takeoff_altitude(10.0f);

    std::cout << "Taking Off: " << "\n";
    const Action::Result takeoff_result = _aAction.takeoff();
    if (takeoff_result != Action::Result::Success) {
        std::cerr << "Takeoff failed: " << takeoff_result << '\n';
        return;
    }

    //sleep_for(seconds(2));
    std::cout << "Flight Mode is: " << _aTelemetry.flight_mode() << '\n';

    // Go somewhere
    const Action::Result goto_result = _aAction.goto_location(47.398000, 8.545592, NAN, NAN);
    std::this_thread::sleep_for(std::chrono::seconds(10));
    if (goto_result != Action::Result::Success) {
        std::cerr << "Goto failed: " << goto_result << '\n';
        return;
    }
    else {
        std::cout << "Goto Succeed." << "\n";
    }

    std::cout << "RTL...\n";
    const Action::Result land_result = _aAction.return_to_launch();
    std::cout << "Flight Mode is: " << _aTelemetry.flight_mode() << '\n';
    if (land_result != Action::Result::Success) {
        std::cerr << "RTL failed: " << land_result << '\n';
        return;
    }
    else {
        std::cout << "RTL Succeed." << "\n";
    }

    // Check if vehicle is still in air
    while (_aTelemetry.in_air()) {
        std::cout << "Vehicle is landing...\n";
        sleep_for(seconds(1));
    }
    std::cout << "Flight Mode is: " << _aTelemetry.flight_mode() << '\n';
    std::cout << "Landed!\n";

    std::cout << "Disarming motors!\n";
    const Action::Result disarm_result = _aAction.disarm();
    if (disarm_result != Action::Result::Success) {
        std::cerr << "Disarm failed: " << land_result << '\n';
        return;
    }
    else{
        std::cout << "Disarmed motors!\n";
    }

    // We are relying on auto-disarming but let's keep watching the telemetry for a bit longer.
    // sleep_for(seconds(3));
    std::cout << "Finished...\n";
}

void test_takeoff_RTL( mavsdk::Telemetry& _aTelemetry,
                        mavsdk::Action& _aAction)
{
    // Check until vehicle is ready to arm
    // while (_aTelemetry.health_all_ok() != true) {
    //     std::cout << "Vehicle is getting ready to arm\n";
    //     std::cout << "Health: " << _aTelemetry.health();
    //     sleep_for(seconds(5));
    // }

    // Arm vehicle
    std::cout << "Arming...\n";
    const Action::Result arm_result = _aAction.arm();

    if (arm_result != Action::Result::Success) {
        std::cerr << "Arming failed: " << arm_result << '\n';
        return;
    }
    std::cout << "Armed.\n" ;
    
    std::cout << "Setting takeoff altitude\n";
    _aAction.set_takeoff_altitude(10.0f);

    std::cout << "Taking Off: " << "\n";
    const Action::Result takeoff_result = _aAction.takeoff();
    if (takeoff_result != Action::Result::Success) {
        std::cerr << "Takeoff failed: " << takeoff_result << '\n';
        return;
    }

    // Let it hover for a bit before landing again.
    //sleep_for(seconds(10));

    std::cout << "RTL...\n";
    const Action::Result land_result = _aAction.return_to_launch();
    std::cout << "Flight Mode is: " << _aTelemetry.flight_mode() << '\n';
    if (land_result != Action::Result::Success) {
        std::cerr << "RTL failed: " << land_result << '\n';
        return;
    }
    else {
        std::cout << "RTL Succeed." << "\n";
    }

    // Check if vehicle is still in air
    while (_aTelemetry.in_air()) {
        std::cout << "Vehicle is landing...\n";
        sleep_for(seconds(1));
    }
    std::cout << "Flight Mode is: " << _aTelemetry.flight_mode() << '\n';
    std::cout << "Landed!\n";

    std::cout << "Disarming motors!\n";
    const Action::Result disarm_result = _aAction.disarm();
    if (disarm_result != Action::Result::Success) {
        std::cerr << "Disarm failed: " << land_result << '\n';
        return;
    }
    else{
        std::cout << "Disarmed motors!\n";
    }

    // We are relying on auto-disarming but let's keep watching the telemetry for a bit longer.
    // sleep_for(seconds(3));
    std::cout << "Finished...\n";
}

void test_takeoff_land( mavsdk::Telemetry& _aTelemetry,
                        mavsdk::Action& _aAction)
{
    // Check until vehicle is ready to arm
    // while (_aTelemetry.health_all_ok() != true) {
    //     std::cout << "Vehicle is getting ready to arm\n";
    //     std::cout << "Health: " << _aTelemetry.health();
    //     sleep_for(seconds(5));
    // }

    _aTelemetry.subscribe_flight_mode([](Telemetry::FlightMode flight_mode) {
        std::cout << "Flight Mode: " << flight_mode << " m\n";
    });

    // Arm vehicle
    std::cout << "Arming...\n";
    const Action::Result arm_result = _aAction.arm();
    if (arm_result != Action::Result::Success) {
        std::cerr << "Arming failed: " << arm_result << '\n';
        return;
    }
    std::cout << "Armed.\n" ;
    
    std::cout << "Setting takeoff altitude\n";
    _aAction.set_takeoff_altitude(10.0f);
    const Action::Result takeoff_result = _aAction.takeoff();
    if (takeoff_result != Action::Result::Success) {
        std::cerr << "Takeoff failed: " << takeoff_result << '\n';
        return;
    }
    
    std::cout << "Takeoff Succeeded." << '\n';

    std::cout << "Landing...\n";
    const Action::Result land_result = _aAction.land();
    if (land_result != Action::Result::Success) {
        std::cerr << "Land failed: " << land_result << '\n';
        return;
    }
    // Check if vehicle is still in air
    while (_aTelemetry.in_air()) {
        std::cout << "Vehicle is landing...\n";
        sleep_for(seconds(1));
    }
    std::cout << "Landed!\n";

    std::cout << "Disarming motors!\n";
    const Action::Result disarm_result = _aAction.disarm();
    if (disarm_result != Action::Result::Success) {
        std::cerr << "Disarm failed: " << land_result << '\n';
        return;
    }
    else{
        std::cout << "Disarmed motors!\n";
    }
    std::cout << "Finished...\n";
}

void test_terminate( mavsdk::Telemetry& _aTelemetry,
                        mavsdk::Action& _aAction)
{
    // Check until vehicle is ready to arm
    // while (_aTelemetry.health_all_ok() != true) {
    //     std::cout << "Vehicle is getting ready to arm\n";
    //     std::cout << "Health: " << _aTelemetry.health();
    //     sleep_for(seconds(5));
    // }
    
    std::cout << "Arming...\n";
    const Action::Result arm_result = _aAction.arm();

    if (arm_result != Action::Result::Success) {
        std::cerr << "Arming failed: " << arm_result << '\n';
        return;
    }
    std::cout << "Armed.\n" ;
    std::cout << "Taking off...\n";
    
    std::cout << "Setting takeoff altitude\n";
    _aAction.set_takeoff_altitude(10.0f);

    const Action::Result takeoff_result = _aAction.takeoff();
    if (takeoff_result != Action::Result::Success) {
        std::cerr << "Takeoff failed: " << takeoff_result << '\n';
        return;
    }

    // Let it hover for a bit before landing again.
    sleep_for(seconds(10));

    // Terminate action:
    const Action::Result terminate_result = _aAction.terminate();
    if (terminate_result != Action::Result::Success) {
        std::cerr << "Terminate failed: " << terminate_result << '\n';
        return;
    }
    else {
        std::cout << "Successfully Terminated." << "\n";
    }
    
    std::cout << "Finished...\n";
}

void test_reboot( mavsdk::Telemetry& _aTelemetry,
                        mavsdk::Action& _aAction)
{
    // Check until vehicle is ready to arm
    // while (_aTelemetry.health_all_ok() != true) {
    //     std::cout << "Vehicle is getting ready to arm\n";
    //     std::cout << "Health: " << _aTelemetry.health();
    //     sleep_for(seconds(5));
    // }
    
    std::cout << "Testing Reboot...\n";
    // Reboot action:
    const Action::Result reboot_result = _aAction.terminate();
    if (reboot_result != Action::Result::Success) {
        std::cerr << "Reboot failed: " << reboot_result << '\n';
        return;
    }
    else {
        std::cout << "Successfully Rebooted." << "\n";
    }
    
    std::cout << "Finished...\n";
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
