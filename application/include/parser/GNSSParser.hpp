#pragma once

#include "misc/GNSS.hpp"

#include <string>

class UBXParser
{
private :
    using Start = size_t;
    using End = size_t;

    std::string _buffer;

public :
    auto parse(const std::string& message) -> GNSS;
    inline auto buffer_length() const noexcept -> size_t { return _buffer.length(); }

private :
    auto find_message() const noexcept -> std::tuple<Start, End, MessageType::type>;

private :
    auto parseNMEA(Start start, End end, GNSS& gnss) -> bool;
    auto parseUBX(GNSS& gnss) -> bool;
};
