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

#include "imu/Imu.hpp"
#include "imu/MPU6050.hpp"

#include "common/logging/Logger.hpp"
#include "math/constant.hpp"

namespace engine
{
Imu::Imu(std::shared_ptr<::common::Serial> serial)
    : _serial(serial) {}

auto Imu::subscribe_update_imuData(std::shared_ptr<::common::BaseObserver<ImuData>> observer) -> void
{
    _updateImuData.regist(std::move(observer));
}

auto Imu::unsubscribe_update_imuData(std::shared_ptr<::common::BaseObserver<ImuData>> observer) -> void
{
    _updateImuData.unregist(std::move(observer));
}

auto Imu::__work() -> void
{
    const std::string msg(_serial->readline());
    const auto [acc, gyr] = MPU6050::parse(msg);

    /********************************************************************************/
    /* Complimentary Filter */
    /********************************************************************************/
    // constexpr double ALPHA = 0.98;
    // constexpr double dt = 0.01;

    // auto roll = atan2(acc._y, acc._z) * math::DEG2RAD;
    // auto pitch = atan(-acc._x / sqrt(acc._y * acc._y + acc._z * acc._z)) * math::DEG2RAD;    

    // _data._x = (1 - ALPHA) * (_data._x + gyr._x * dt) + ALPHA * roll;
    // _data._y = (1 - ALPHA) * (_data._y + gyr._y * dt) + ALPHA * roll;
    /********************************************************************************/

    /********************************************************************************/
    /* Kalman Filter */
    /*****************************************4***************************************/
    constexpr double dt = 0.01; // 100Hz

    const auto accRoll = atan2(acc._y, acc._z);
    const auto orgPitch = (acc._y * acc._y) + (acc._z * acc._z);
    const auto accPitch = orgPitch == 0 ? 0 : atan(-acc._x / sqrt(orgPitch));

    const auto gyrRoll = gyr._x * ::common::math::DEG2RAD;
    const auto gyroPitch = gyr._y * ::common::math::DEG2RAD;
    const auto gyrYaw = gyr._z * ::common::math::DEG2RAD;

    _data._x = _filterRoll.run(accRoll, _data._x + gyrRoll * dt);
    _data._y = _filterRoll.run(accPitch, _data._y + gyroPitch * dt);
    /********************************************************************************/    

    _data._z = _yawCal.calculate(gyr._z, acc._x, acc._y, dt);

    _updateImuData.notify(_data);
}

auto Imu::__create(std::shared_ptr<::common::Serial> serial) -> std::shared_ptr<Imu>
{
    return std::make_shared<Imu>(serial);
}
} // namespace engine