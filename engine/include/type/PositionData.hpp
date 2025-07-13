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

#include "common/Observer.hpp"

namespace engine
{
struct PositionData
{
    uint32_t _type = 0x0;
    double _utc = 0.0;
    double _longitude = 0.0;
    double _latitude = 0.0;
    int32_t _altitude = 0;
};

class EventUpdatePositionData : public ::common::Subject<PositionData> {};
} // namespace engine