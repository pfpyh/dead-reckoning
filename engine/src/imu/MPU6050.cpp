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

#include "imu/MPU6050.hpp"
#include "common/logging/Logger.hpp"

#include <sstream>

namespace engine
{
auto stoi(const std::string& str) -> int32_t
{
    if (str.empty()) { return 0; }

    size_t i = 0;
    bool negative = false;
    if (str[i] == '-') 
    {
        negative = true;
        i++;
    }   

    int32_t result = 0;
    bool found_digit = false;
    for (; i < str.size(); ++i) 
    {
        if (str[i] < 0) { return 0; }
        if (std::isdigit(str[i])) 
        {
            result = result * 10 + (str[i] - '0');
            found_digit = true;
        }
    }

    return found_digit ? (negative ? -result : result) : 0;
}
    
auto MPU6050::parse(const std::string& message) noexcept -> std::tuple<AccData, GyroData>
{
    std::vector<int32_t> values;
    std::string token;
    std::istringstream iss(message);
    while(std::getline(iss, token, ' '))
    {
        const auto value = ::engine::stoi(token);
        values.push_back(value);
    }

    if(values.size() != 6)
    {
        return {AccData(), GyroData()};
    }

    AccData accData;
    accData._x = static_cast<double>(values[0] / 16384);
    accData._y = static_cast<double>(values[1] / 16384);
    accData._z = static_cast<double>(values[2] / 16384);

    GyroData gyroData;
    gyroData._x = static_cast<double>(values[3] / 131);
    gyroData._y = static_cast<double>(values[4] / 131);
    gyroData._z = static_cast<double>(values[5] / 131);

    return {accData, gyroData};
}
} // namespace engine