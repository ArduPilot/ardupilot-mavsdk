//
// Simple example to demonstrate how takeoff and land using MAVSDK.
//

#include <chrono>
#include <cstdint>
#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/param/param.h>
#include <iostream>
#include <future>
#include <memory>
#include <thread>
//#include "integration_test_helper.h"

using namespace mavsdk;
using std::chrono::seconds;
using std::this_thread::sleep_for;

void usage(const std::string& bin_name);

void test_param_set(mavsdk::Param& param);


int main(int argc, char** argv)
{
    if (argc != 2) {
        usage(argv[0]);
        return 1;
    }

    Mavsdk mavsdk;
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
    auto param = Param{system.value()};

    // first get all params from the autopilot and print them.
    // Param::AllParams _all_params = param.get_all_params();
    // std::cout << "Params: " << _all_params << " m\n";

    test_param_set(param);

    return 0;
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

void test_param_set(mavsdk::Param& param)
{
    // Check for Parameter Setting for all parameters which were obtained using get_all_params()
    
    {
        const std::pair<Param::Result, int32_t> get_result = param.get_param_int("COMPASS_USE"); // 1
        std::cout << "COMPASS_USE " << "Get Result " << get_result.first << " Value " << get_result.second << "\n";

        const Param::Result set_result = param.set_param_int("COMPASS_USE", 1); // 0
        std::cout << "COMPASS_USE " << "Set Result " << set_result << "\n";

        const std::pair<Param::Result, int32_t> get_result2 = param.get_param_int("COMPASS_USE"); // 0
        std::cout << "COMPASS_USE " << "Get Result " << get_result2.first << " Value " << get_result2.second << "\n";
    } // 1
    
    {
        const std::pair<Param::Result, int32_t> get_result = param.get_param_int("SERVO1_REVERSED"); // 0
        std::cout << "SERVO1_REVERSED " << "Get Result " << get_result.first << " Value " << get_result.second << "\n";

        const Param::Result set_result = param.set_param_int("SERVO1_REVERSED", 1);
        std::cout << "SERVO1_REVERSED " << "Set Result " << set_result << "\n";

        const std::pair<Param::Result, int32_t> get_result2 = param.get_param_int("SERVO1_REVERSED"); // 1
        std::cout << "SERVO1_REVERSED " << "Get Result " << get_result2.first << " Value " << get_result2.second << "\n";
    } // 0

    {
        const std::pair<Param::Result, int32_t> get_result = param.get_param_int("RC10_MIN"); //1100
        std::cout << "RC10_MIN " << "Get Result " << get_result.first << " Value " << get_result.second << "\n";

        const Param::Result set_result = param.set_param_int("RC10_MIN", 1200);
        std::cout << "RC10_MIN " << "Set Result " << set_result << "\n";

        const std::pair<Param::Result, int32_t> get_result2 = param.get_param_int("RC10_MIN"); //1200
        std::cout << "RC10_MIN " << "Get Result " << get_result2.first << " Value " << get_result2.second << "\n";
    } // 1100
    
    {
        const std::pair<Param::Result, int32_t> get_result = param.get_param_int("LAND_ALT_LOW"); // 1000
        std::cout << "LAND_ALT_LOW " << "Get Result " << get_result.first << " Value " << get_result.second << "\n";

        const Param::Result set_result = param.set_param_int("LAND_ALT_LOW", 1200);
        std::cout << "LAND_ALT_LOW " << "Set Result " << set_result << "\n";

        const std::pair<Param::Result, int32_t> get_result2 = param.get_param_int("LAND_ALT_LOW"); // 1200
        std::cout << "LAND_ALT_LOW " << "Get Result " << get_result2.first << " Value " << get_result2.second << "\n";
    } // 1000

    {
        const std::pair<Param::Result, int32_t> get_result = param.get_param_int("SERIAL1_BAUD"); // 57
        std::cout << "SERIAL1_BAUD " << "Get Result " << get_result.first << " Value " << get_result.second << "\n";

        const Param::Result set_result = param.set_param_int("SERIAL1_BAUD", 115);
        std::cout << "SERIAL1_BAUD " << "Set Result " << set_result << "\n";

        const std::pair<Param::Result, int32_t> get_result2 = param.get_param_int("SERIAL1_BAUD"); // 115
        std::cout << "SERIAL1_BAUD " << "Get Result " << get_result2.first << " Value " << get_result2.second << "\n";
    } // 115
    

    {
        const std::pair<Param::Result, float> get_result = param.get_param_float("WPNAV_SPEED"); // 1000
        std::cout << "WPNAV_SPEED " << "Get Result " << get_result.first << " Value " << get_result.second << "\n";

        const Param::Result set_result = param.set_param_float("WPNAV_SPEED", 1100);
        std::cout << "WPNAV_SPEED " << "Set Result " << set_result << "\n";

        const std::pair<Param::Result, float> get_result2 = param.get_param_float("WPNAV_SPEED"); // 1100
        std::cout << "WPNAV_SPEED " << "Get Result " << get_result2.first << " Value " << get_result2.second << "\n";
    } // 1000
    
    {
        const std::pair<Param::Result, float> get_result = param.get_param_float("AHRS_YAW_P"); // 0.2
        std::cout << "AHRS_YAW_P " << "Get Result " << get_result.first << " Value " << get_result.second << "\n";

        const Param::Result set_result = param.set_param_float("AHRS_YAW_P", 0.4);
        std::cout << "AHRS_YAW_P " << "Set Result " << set_result << "\n";

        const std::pair<Param::Result, float> get_result2 = param.get_param_float("AHRS_YAW_P"); // 0.4
        std::cout << "AHRS_YAW_P " << "Get Result " << get_result2.first << " Value " << get_result2.second << "\n";
    }
}