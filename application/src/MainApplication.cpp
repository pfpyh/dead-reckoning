#include "MainApplication.hpp"

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

    auto holder = _uart.get(UartType::IMU);
    holder->reading([](const std::string& message){
        _INFO_("Recv={%s}", message.c_str());
    });

    return nullptr;
}

auto MainApplication::shutdown() -> ::common::Future
{
    _uart.finalize();
    return nullptr;
}