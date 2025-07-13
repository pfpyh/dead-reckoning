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

#pragma once

#include "type/PositionData.hpp"

#include <string>

namespace engine
{
class NeoM6
{
public :
    class SupportType
    {
    public :
        enum type : uint32_t
        {
            GPGGA = 0x0001,
            GPRMC = 0x0002,
            GPVTG = 0x0004,
        };
    };

public :
    static auto parse(const std::string& message) noexcept -> PositionData;
};
} // engine