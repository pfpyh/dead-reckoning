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

#include "type/PositionData.hpp"

namespace engine
{
class Gnss : public ::common::Runnable
           , public ::common::Factory<Gnss>
{
    friend class ::common::Factory<Gnss>;

private :
    std::shared_ptr<::common::Serial> _serial;
    EventUpdatePositionData _updatePosition;
    PositionData _position;

public :
    Gnss(std::shared_ptr<::common::Serial> serial);

public :
    auto subscribe_update_position(std::shared_ptr<::common::BaseObserver<PositionData>> observer) -> void;
    auto unsubscribe_update_position(std::shared_ptr<::common::BaseObserver<PositionData>> observer) -> void;

private :
    auto __work() -> void override;
    static auto __create(std::shared_ptr<::common::Serial> serial) -> std::shared_ptr<Gnss>;
};
} // namespace engine