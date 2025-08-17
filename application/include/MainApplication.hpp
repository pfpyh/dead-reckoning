#pragma once

#include "common/lifecycle/Application.h"

#include "controller/UartController.hpp"

class MainApplication : public ::common::Application
{
private :
    UartController _uart;

public :
    auto bootup() -> ::common::Future override;
    auto shutdown() -> ::common::Future override;
};