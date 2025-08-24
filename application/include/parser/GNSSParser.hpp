#pragma once

#include "misc/GNSS.hpp"

#include <string>

class UBXParser
{
private :
    std::string _buffer;

public :
    auto parse(const std::string& message) -> GNSS;

private :
    auto parseNMEA(GNSS& gnss) -> bool;
    auto parseUBX(GNSS& gnss) -> bool;
};
