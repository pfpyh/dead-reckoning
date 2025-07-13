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
#include "common/thread/Runnable.hpp"
#include "common/Factory.hpp"
#include "common/communication/Serial.hpp"

#include "math/filter/KalmanFilter.hpp"

#include "type/ImuData.hpp"

namespace engine
{
namespace detail
{
class YawCalculator
{
private :
    double _prevAccYaw = 0.0;
    double _prevGyrYaw = 0.0;
    const double _alpha = 0.98;

public :
    auto calculate(double gyrZ, double accX, double accY, double dt) -> double
    {
        const auto accYaw = atan2(accY, accX);
        const auto accYawRate = (accYaw - _prevAccYaw) / dt;
        const auto fusionYawRate = _alpha * gyrZ + (1 - _alpha) * accYawRate;
        _prevAccYaw = accYaw;
        return fusionYawRate;
    }
};
} // namespace detail

class Imu : public ::common::Runnable
          , public ::common::Factory<Imu>
{
    friend class ::common::Factory<Imu>;

private :
    std::shared_ptr<::common::Serial> _serial;

    ::common::math::KalmanFilter<double> _filterRoll{0.1, 1.0, 1.0};
    ::common::math::KalmanFilter<double> _filterPitch{0.1, 1.0, 1.0};
    ::common::math::KalmanFilter<double> _filterYaw{0.1, 1.0, 1.0};

    detail::YawCalculator _yawCal;


    EventUpdateImuData _updateImuData;
    ImuData _data;

public :
    Imu(std::shared_ptr<::common::Serial> serial);

public :
    auto subscribe_update_imuData(std::shared_ptr<::common::BaseObserver<ImuData>> observer) -> void;
    auto unsubscribe_update_imuData(std::shared_ptr<::common::BaseObserver<ImuData>> observer) -> void;

private :
    auto __work() -> void override;
    static auto __create(std::shared_ptr<::common::Serial> serial) -> std::shared_ptr<Imu>;
};
} // namespace engine