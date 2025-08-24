#include "MainApplication.hpp"
#include "parser/IMUParser.hpp"
#include "parser/GNSSParser.hpp"

#include "common/logging/Logger.hpp"
#include "common/utils/String.hpp"

#include <filesystem>

auto MainApplication::bootup() -> ::common::Future
{
    std::string confPath(UART_CONF_PATH);
    if(!std::filesystem::exists(confPath)) {
        confPath = "uart_conf";
    }

    auto uartConfs = UartController::read_configuration(confPath);
    for(auto& conf : uartConfs)
    {
        auto serial = ::common::Serial::create();
        if(serial->open(conf._port, conf._baudrate, conf._mode))
        {
            _uart.set(conf._type, serial);
        }
    }

    try
    {
        _imuHandler.initialize(_uart.get(UartType::IMU),
                               parse_imu_dummy,
                               [](const IMU& imu) {
            _INFO_("Gyro[%f, %f, %f] ACC[%f, %f, %f]", 
                imu._gyro._roll, imu._gyro._pitch, imu._gyro._yaw,
                imu._acc._x, imu._acc._y, imu._acc._z);
        });
    }
    catch(std::invalid_argument& ex) { _DEBUG_("[IMU] is not connected!"); }

    try
    {
        _gnssHandler.initialize(_uart.get(UartType::GNSS),
                                [](const std::string& message) -> GNSS {
                                    static GNSSParser parser;
                                    static std::mutex lock;

                                    std::lock_guard<std::mutex> scopedLock(lock);
                                    return parser.parse(message);
                                },
                                [](const GNSS& gnss) {
            switch (gnss._type)
            {
            case MessageType::NMEA:
                {
                    _INFO_("NMEA [%s]", gnss._message.c_str());
                }
                break;
            case MessageType::UBX:
                {
                    const auto hex = ::common::utils::string_to_hex(gnss._message);
                    _INFO_("UBX [%s]", hex.c_str());
                }
                break;
            default:
                break;
            }
        });
    }
    catch(std::invalid_argument& ex) { _DEBUG_("[GNSS] is not connected!"); }

    return nullptr;
}

auto MainApplication::shutdown() -> ::common::Future
{
    _uart.finalize();
    return nullptr;
}