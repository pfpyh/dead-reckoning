#include "parser/GNSSParser.hpp"

auto UBXParser::parse(const std::string& message) -> GNSS
{
    _buffer += message;

    GNSS gnss;
    if(parseNMEA(gnss)) { return gnss; }
    if(parseUBX(gnss)) { return gnss; }
    return gnss;
}

auto UBXParser::parseNMEA(GNSS& gnss) -> bool
{
    size_t start_pos = _buffer.find("$GP");
    if (start_pos == std::string::npos) { return false; }
    
    size_t end_pos = _buffer.find('\r', start_pos);
    if (end_pos == std::string::npos) { return false; }
    
    gnss._message = _buffer.substr(start_pos, end_pos - start_pos + 1);
    gnss._type = MessageType::NMEA;
    
    _buffer.erase(0, end_pos + 1);
    return true;
}

auto UBXParser::parseUBX(GNSS& gnss) -> bool
{
    for (size_t i = 0; i < _buffer.size() - 1; ++i)
    {
        if (static_cast<unsigned char>(_buffer[i]) == 0xB5 && 
            static_cast<unsigned char>(_buffer[i + 1]) == 0x62)
        {
            _buffer.erase(0, i);
            gnss._type = MessageType::UBX;
            return true;
        }
    }
    return false;
}