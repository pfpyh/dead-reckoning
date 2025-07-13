/*
 * GPL v3 License
 * 
 * Copyright (c) 2025 Park Younghwan
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "gnss/NeoM6.hpp"

#include <sstream>

namespace engine
{
auto string_split(const std::string& message) -> std::vector<std::string>
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream iss(message);
    while(std::getline(iss, token, ','))
    {
        tokens.push_back(token);
    }
    return tokens;
}

auto to_decimal_degree(const std::string& degreesMinutes, const std::string& direction) -> double
{
    if (degreesMinutes.empty()) { return 0.0; }

    auto dotPos = degreesMinutes.find('.');
    int32_t degrees = std::stoi(degreesMinutes.substr(0, dotPos - 2));
    double minutes = std::stod(degreesMinutes.substr(dotPos - 2));
    double decimalDegrees = degrees + (minutes / 60.0);

    if (direction == "S" || direction == "W") {
        decimalDegrees *= -1;
    }
    return decimalDegrees;
}

auto GPGGA(const std::vector<std::string>& tokens, PositionData& data) -> bool
{
    if (tokens.size() > 5 && tokens[0] == "$GPGGA")
    {
        data._utc = std::stod(tokens[1]);
        data._latitude = to_decimal_degree(tokens[2], tokens[3]);
        data._longitude = to_decimal_degree(tokens[4], tokens[5]);
        return true;
    }
    return false;
}

auto GPRMC(const std::vector<std::string>& tokens, PositionData& data) -> bool
{
    return false;
}

auto GPVTG(const std::vector<std::string>& tokens, PositionData& data) -> bool
{
    return false;
}

auto NeoM6::parse(const std::string& message) noexcept -> PositionData
{
    const std::vector<std::string> tokens(string_split(message));

    PositionData data;

    if(GPGGA(tokens, data))
    {
        data._type = (data._type | SupportType::GPGGA);
    }

    if(GPRMC(tokens, data))
    {
        data._type = (data._type | SupportType::GPRMC);
    }

    if(GPVTG(tokens, data))
    {
        data._type = (data._type | SupportType::GPVTG);
    }

    return data;
}
} // namespace engine