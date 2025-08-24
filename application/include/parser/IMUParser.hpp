#pragma once

#include "misc/IMU.hpp"

#include <cstdlib>
#include <array>

#include "common/logging/Logger.hpp"

auto parse_MPU6050(const std::string& message) -> IMU
{
    return IMU();
}

auto parse_imu_dummy(const std::string& message) -> IMU
{
    std::array<double, 6> list;

    const char* ptr = message.c_str();
    char* endptr;

    for(auto& field : list)
    {
        field = strtod(ptr, &endptr);
        if (ptr == endptr) 
        { 
            _ERROR_("Parsing error");
            return IMU();
        }
        ptr = endptr;
    }
    
    IMU imu;
    imu._acc._x = list[0];
    imu._acc._y = list[1];
    imu._acc._z = list[2];
    imu._gyro._roll = list[3];
    imu._gyro._pitch = list[4];
    imu._gyro._yaw = list[5];
    return imu;
}