#pragma once

struct Gyro
{
    double _roll = 0.0;
    double _pitch = 0.0;
    double _yaw = 0.0;
};

struct Acc
{
    double _x = 0.0;
    double _y = 0.0;
    double _z = 0.0;
};

struct IMU
{
    Gyro _gyro;
    Acc _acc;
};