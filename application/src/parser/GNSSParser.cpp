#include "parser/GNSSParser.hpp"

#include "common/logging/Logger.hpp"

auto GNSSParser::parse(const std::string& message) -> GNSS
{
    _buffer += message;

    auto[start, end, type] = find_message();
    if(start == std::string::npos) { return GNSS(); }

    GNSS gnss;
    if(type == MessageType::NMEA && !parseNMEA(start, end, gnss))
    {
        _ERROR_("NMEA parser failure");
        return GNSS();
    }
    else if(type == MessageType::UBX && !parseUBX(start, end, gnss))
    {
        _ERROR_("UBX parser failure");
        return GNSS();
    }

    // compact buffer with corrupted message
    if(end != std::string::npos) { _buffer.erase(0, end); }
    return gnss;
}

auto GNSSParser::find_message() const noexcept -> std::tuple<Start, End, MessageType::type>
{
    const auto end = _buffer.size() - 1;
    for(size_t i = 0; i < end; ++i)
    {
        // Case.1 UBX message
        // [SYN (0xb5)][SYN (0x62)][CLASS (1btye)][ID (1byte)][LENGTH (2byte)]
        if((i + 5) < end &&
           static_cast<unsigned char>(_buffer[i]) == 0xb5 &&
           static_cast<unsigned char>(_buffer[i + 1] == 0x62))
        {
            const uint16_t length = (static_cast<uint8_t>(_buffer[i + 4])) |
                                    (static_cast<uint8_t>(_buffer[i + 5]) << 8);
            if((i + 5 + length) < end)
            {
                return {i, i + 5 + length, MessageType::UBX};
            }
        }
        // Case.2 NMEA
        else if((i + 2) < end &&
                _buffer[i] == '$' && _buffer[i + 1] == 'G' && _buffer[i + 2] == 'P')
        {
            auto end = _buffer.find(LINE_END, i);
            if(end != std::string::npos)
            {
                return {i, end, MessageType::NMEA};
            }
        }
        else if((i + 2) < end &&
                _buffer[i] == '$' && _buffer[i + 1] == 'G' && _buffer[i + 2] == 'L')
        {
            auto end = _buffer.find(LINE_END, i);
            if(end != std::string::npos)
            {
                return {i, end, MessageType::NMEA};
            }
        }
        else if((i + 2) < end &&
                _buffer[i] == '$' && _buffer[i + 1] == 'G' && _buffer[i + 2] == 'N')
        {
            auto end = _buffer.find(LINE_END, i);
            if(end != std::string::npos)
            {
                return {i, end, MessageType::NMEA};
            }
        }
    }
    return {std::string::npos, std::string::npos, MessageType::UNKNOWN};
}

auto GNSSParser::parseNMEA(Start start, End end, GNSS& gnss) -> bool
{
    gnss._message = _buffer.substr(start, (end - start));
    gnss._type = MessageType::NMEA;
    return true;
}

auto GNSSParser::parseUBX(Start start, End end, GNSS& gnss) -> bool
{
    gnss._message = _buffer.substr(start, (end - start));
    gnss._type = MessageType::UBX;
    return true;
}