#pragma once

#include <string>

struct MessageType
{
    enum type : uint8_t
    {
        UNKNOWN,
        NMEA,
        UBX,
    };
};

struct GNSS
{
    MessageType::type _type = MessageType::UNKNOWN;
    int32_t _lat = 0;
    int32_t _long = 0;
    std::string _message;
};