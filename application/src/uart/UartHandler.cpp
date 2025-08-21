#include "uart/UartHandler.hpp"

std::shared_ptr<::common::TaskExecutor> UartHandler::_executor = nullptr;

UartHandler::UartHandler(std::shared_ptr<::common::Serial> serial) noexcept
    : _serial(std::move(serial)) 
{
    if(!_executor) { _executor = ::common::TaskExecutor::create(2); }
}

UartHandler::~UartHandler() noexcept
{
    _serial->close();
}

auto UartHandler::receiving(std::function<void(const std::string&)>&& onReceiveListener) -> void
{
    _listenerBuffer.add(std::move(onReceiveListener));
}

auto UartHandler::writing(const std::string& message) noexcept -> bool
{
    return _serial->write(message.c_str(), message.length());
}

auto UartHandler::__work() -> void
{
    auto recv(_serial->readline());
    if(!recv.empty())
    {
        auto listeners = _listenerBuffer.get_buffer();
        for(auto& listener : (*listeners)) 
        {
            _executor->load<void>([listener, recv](){
                listener(recv); 
            });
        }
    }
}