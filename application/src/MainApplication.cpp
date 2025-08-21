#include "MainApplication.hpp"
#include "parser/IMUParser.hpp"

#include "common/logging/Logger.hpp"

auto MainApplication::bootup() -> ::common::Future
{
    auto uartConfs = UartController::read_configuration("uart_conf.txt");
    for(auto& conf : uartConfs)
    {
        auto serial = ::common::Serial::create();
        if(serial->open(conf._port, conf._baudrate, conf._mode))
        {
            _uart.set(conf._type, serial);
        }
    }

    _imuHandler.initialize(_uart.get(UartType::IMU),
                           parse_Dummy,
                           [](const IMU& imu) {
        _INFO_("Gyro[%f, %f, %f] ACC[%f, %f, %f]", 
               imu._gyro._roll, imu._gyro._pitch, imu._gyro._yaw,
               imu._acc._x, imu._acc._y, imu._acc._z);
    });
    return nullptr;
}

auto MainApplication::shutdown() -> ::common::Future
{
    _uart.finalize();
    return nullptr;
}