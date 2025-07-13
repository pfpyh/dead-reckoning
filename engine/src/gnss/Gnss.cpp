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

#include "gnss/Gnss.hpp"
#include "gnss/NeoM6.hpp"

namespace engine
{
Gnss::Gnss(std::shared_ptr<::common::Serial> serial)
    : _serial(serial) {}

auto Gnss::subscribe_update_position(std::shared_ptr<::common::BaseObserver<PositionData>> observer) -> void
{
    _updatePosition.regist(std::move(observer));
}

auto Gnss::unsubscribe_update_position(std::shared_ptr<::common::BaseObserver<PositionData>> observer) -> void
{
    _updatePosition.unregist(std::move(observer));
}

auto Gnss::__work() -> void
{
    const std::string msg(_serial->readline());
    auto position = NeoM6::parse(msg);
    if(position._type & NeoM6::SupportType::GPGGA)
    {
        _updatePosition.notify(position);
    }
}

auto Gnss::__create(std::shared_ptr<::common::Serial> serial) -> std::shared_ptr<Gnss>
{
    return std::make_shared<Gnss>(serial);
}
} // namespace engine