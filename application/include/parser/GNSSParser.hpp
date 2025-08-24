#pragma once

#include "misc/GNSS.hpp"

#include <string>

class GNSSParser
{
private :
    using Start = size_t;
    using End = size_t;

    std::string _buffer;
#if defined(WINDOWS)
    static constexpr char LINE_END[3] = {"\r\n"};
#else
    static constexpr char LINE_END[2] = {"\n"};
#endif

public :
    auto parse(const std::string& message) -> GNSS;
    inline auto buffer_length() const noexcept -> size_t { return _buffer.length(); }

private :
    auto find_message() const noexcept -> std::tuple<Start, End, MessageType::type>;

private :
    auto parseNMEA(Start start, End end, GNSS& gnss) -> bool;
    auto parseUBX(Start start, End end, GNSS& gnss) -> bool;
};
