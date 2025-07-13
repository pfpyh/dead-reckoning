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
struct ImuRawData
{
    double _x = 0.0;
    double _y = 0.0;
    double _z = 0.0;
};

using GyroData = ImuRawData; /* unit = g */
using AccData = ImuRawData; /* unit = deg/s */
using ImuData = ImuRawData;

class EventUpdateImuData : public ::common::Subject<ImuData> {};
} // namespace engine