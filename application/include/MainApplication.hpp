#pragma once

#include "common/lifecycle/Application.h"

#include "uart/UartController.hpp"
#include "uart/UartMessageHandler.hpp"

#include "misc/IMU.hpp"

class MainApplication : public ::common::Application
{
private :
    UartController _uart;
    UartMessageHandler<IMU> _imuHandler;

public :
    auto bootup() -> ::common::Future override;
    auto shutdown() -> ::common::Future override;
};