//
// Simple example to demonstrate how takeoff and land using MAVSDK.
//

#include <chrono>
#include <cstdint>
#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/param/param.h>
#include <mavsdk/plugins/info/info.h>
#include <mavsdk/plugins/telemetry/telemetry.h>
#include <iostream>
#include <future>
#include <memory>
#include <thread>
//#include "integration_test_helper.h"

using namespace mavsdk;
using std::chrono::seconds;
using std::this_thread::sleep_for;

void usage(const std::string& bin_name);
void test_set_rates(mavsdk::Telemetry& _aTelemetry);
void test_subscribe(mavsdk::Telemetry& _aTelemetry);
void test_info(mavsdk::Info& _aInfo);




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
    auto info = Info{system.value()};

    test_set_rates(telemetry);
    test_subscribe(telemetry);
    //test_info(info);

    while (true) {
        std::cout << "Getting Data...\n";
        sleep_for(seconds(5));
    }
    return 0;
}

void test_info(mavsdk::Info& _aInfo)
{
    std::cout << "Flight version: " << std::endl;
    // FIXME: we need to wait some time until Info has determined the version.
    std::this_thread::sleep_for(std::chrono::seconds(10));

    std::pair<Info::Result, Info::Version> version_result = _aInfo.get_version();

    if (version_result.first == Info::Result::Success) {
        std::cout << "Flight version: " << version_result.second.flight_sw_major << "."
                    << version_result.second.flight_sw_minor << "."
                    << version_result.second.flight_sw_patch << " ("
                    << std::string(version_result.second.flight_sw_git_hash) << ")" << '\n';    // 4.1.2 (613237376)
        std::cout << "Flight vendor version: " << version_result.second.flight_sw_vendor_major
                    << "." << version_result.second.flight_sw_vendor_minor << "."
                    << version_result.second.flight_sw_vendor_patch << '\n';                    // 98.51.48
        std::cout << "OS version: " << version_result.second.os_sw_major << "."
                    << version_result.second.os_sw_minor << "."
                    << version_result.second.os_sw_patch << " ("
                    << std::string(version_result.second.os_sw_git_hash) << ")" << '\n'; // 0.0.0.0
    } else {
        std::cout << "Version request result: " << version_result.first;
    }

    std::pair<Info::Result, Info::Product> product_result = _aInfo.get_product();

    if (product_result.first == Info::Result::Success) {
        std::cout << "Vendor: " << product_result.second.vendor_name << '\n';  // Undefined
        std::cout << "Product: " << product_result.second.product_name << '\n'; // Undefined
    } else {
        std::cout << "get_product() request result: " << product_result.first;
    }

    std::pair<Info::Result, Info::Identification> identification_result =
        _aInfo.get_identification();

    if (identification_result.first == Info::Result::Success) {
        std::cout << "Hardware UID: " << identification_result.second.hardware_uid << '\n'; // 616230353666303031323866343132303800
    } else {
        std::cout << "get_identification() request result: " << identification_result.first;
    }

    for (unsigned i = 0; i < 3; ++i) {
        std::pair<Info::Result, Info::FlightInfo> flight_info_result = _aInfo.get_flight_information(); // This command does not work.

        if (flight_info_result.first == Info::Result::Success) {
            std::cout << "Time since boot (ms): "
                        << std::to_string(flight_info_result.second.time_boot_ms) << '\n';
            std::cout << "Flight UID: " << flight_info_result.second.flight_uid << '\n';
        } else {
            std::cout << "get_flight_information() request result: " << flight_info_result.first;
        }
    }
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


void test_subscribe(mavsdk::Telemetry& _aTelemetry)
{
    _aTelemetry.subscribe_position([](Telemetry::Position position) {
        std::cout << "Position: " << position << " m\n";
    });

    _aTelemetry.subscribe_position_velocity_ned([](Telemetry::PositionVelocityNed position) {
        std::cout << "PositionVelocityNed: " << position.position << " m\n";
    });

    _aTelemetry.subscribe_home([](Telemetry::Position position) {
        std::cout << "Home: " << position << " m\n";
    });

    _aTelemetry.subscribe_in_air([](bool position) {
        std::cout << "InAir: " << position << " m\n";
    });

    _aTelemetry.subscribe_status_text([](Telemetry::StatusText position) {
        std::cout << "StatusText: " << position << " m\n";
    });

    _aTelemetry.subscribe_armed([](bool position) {
        std::cout << "Armed: " << position << " m\n";
    });

    _aTelemetry.subscribe_attitude_quaternion([](Telemetry::Quaternion position) {
        std::cout << "Att Quat: " << position << " m\n";
    });

    _aTelemetry.subscribe_attitude_euler([](Telemetry::EulerAngle position) {
        std::cout << "Att Euler: " << position << " m\n";
    });

    _aTelemetry.subscribe_attitude_angular_velocity_body([](Telemetry::AngularVelocityBody position) {
        std::cout << "Att Angular Velocity: " << position << " m\n";
    });

    _aTelemetry.subscribe_attitude_euler([](Telemetry::EulerAngle position) {
        std::cout << "Att Euler: " << position << " m\n";
    });

    _aTelemetry.subscribe_attitude_euler([](Telemetry::EulerAngle position) {
        std::cout << "Att Euler: " << position << " m\n";
    });

    _aTelemetry.subscribe_velocity_ned([](Telemetry::VelocityNed position) {
        std::cout << "Velocity NED: " << position << " m\n";
    });

    _aTelemetry.subscribe_imu([](Telemetry::Imu position) {
        std::cout << "IMU: " << position << " m\n";
    });

    _aTelemetry.subscribe_scaled_imu([](Telemetry::Imu position) {
        std::cout << "Scaled IMU: " << position << " m\n";
    });

    _aTelemetry.subscribe_raw_imu([](Telemetry::Imu position) {
        std::cout << "Raw IMU: " << position << " m\n";
    });

    _aTelemetry.subscribe_gps_info([](Telemetry::GpsInfo position) {
        std::cout << "GPS Info: " << position << " m\n";
    });

    _aTelemetry.subscribe_raw_gps([](Telemetry::RawGps position) {
        std::cout << "Raw GPS Info: " << position << " m\n";
    });

    _aTelemetry.subscribe_battery([](Telemetry::Battery position) {
        std::cout << "Battery: " << position << " m\n";
    });

    _aTelemetry.subscribe_flight_mode([](Telemetry::FlightMode position) {
        std::cout << "Flight Mode: " << position << " m\n";
    });

    _aTelemetry.subscribe_health([](Telemetry::Health position) {
        std::cout << "Health: " << position << " m\n";
    });

    _aTelemetry.subscribe_health_all_ok([](bool position) {
        std::cout << "Health All Ok: " << position << " m\n";
    });

    _aTelemetry.subscribe_landed_state([](Telemetry::LandedState position) {
        std::cout << "Landed State: " << position << " m\n";
    });

    _aTelemetry.subscribe_distance_sensor([](Telemetry::DistanceSensor position) {
        std::cout << "Distance sensor: " << position << " m\n";
    });

}


void test_set_rates(mavsdk::Telemetry& _aTelemetry)
{
    {
        const auto set_rate_result = _aTelemetry.set_rate_position_velocity_ned(1.0);
        if (set_rate_result != Telemetry::Result::Success) {
            std::cerr << "Setting set_rate_position_velocity_ned failed: " << set_rate_result << '\n';
        }
    }
    {
        const auto set_rate_result = _aTelemetry.set_rate_position(1.0);
        if (set_rate_result != Telemetry::Result::Success) {
            std::cerr << "Setting set_rate_position failed: " << set_rate_result << '\n';
        }
    }
    {
        const auto set_rate_result = _aTelemetry.set_rate_home(1.0);
        if (set_rate_result != Telemetry::Result::Success) {
            std::cerr << "Setting set_rate_home failed: " << set_rate_result << '\n';
        }
    }
    {
        const auto set_rate_result = _aTelemetry.set_rate_in_air(1.0);
        if (set_rate_result != Telemetry::Result::Success) {
            std::cerr << "Setting set_rate_in_air failed: " << set_rate_result << '\n';
        }
    }
    {
        const auto set_rate_result = _aTelemetry.set_rate_landed_state(1.0);
        if (set_rate_result != Telemetry::Result::Success) {
            std::cerr << "Setting set_rate_landed_state failed: " << set_rate_result << '\n';
        }
    }
    {
        const auto set_rate_result = _aTelemetry.set_rate_velocity_ned(1.0);
        if (set_rate_result != Telemetry::Result::Success) {
            std::cerr << "Setting set_rate_velocity_ned failed: " << set_rate_result << '\n';
        }
    }
    {
        const auto set_rate_result = _aTelemetry.set_rate_gps_info(1.0);
        if (set_rate_result != Telemetry::Result::Success) {
            std::cerr << "Setting set_rate_gps_info failed: " << set_rate_result << '\n';
        }
    }
    {
        const auto set_rate_result = _aTelemetry.set_rate_battery(1.0);
        if (set_rate_result != Telemetry::Result::Success) {
            std::cerr << "Setting set_rate_battery failed: " << set_rate_result << '\n';
        }
    }
    {
        const auto set_rate_result = _aTelemetry.set_rate_position(1.0);
        if (set_rate_result != Telemetry::Result::Success) {
            std::cerr << "Setting set_rate_position failed: " << set_rate_result << '\n';
        }
    }
    {
        const auto set_rate_result = _aTelemetry.set_rate_distance_sensor(1.0);
        if (set_rate_result != Telemetry::Result::Success) {
            std::cerr << "Setting set_rate_distance_sensor failed: " << set_rate_result << '\n';
        }
    }
   
}