#pragma once

#include "misc/GNSS.hpp"

#include <string>

auto parse_ubx_6m(const std::string& message) -> GNSS
{
    return GNSS();
}

auto parser_ubx_dummy(const std::string& message) -> GNSS
{
    return GNSS();
}