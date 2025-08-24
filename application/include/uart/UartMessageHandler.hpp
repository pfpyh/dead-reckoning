#pragma once

#include "uart/UartHandler.hpp"

template <typename DataType>
class UartMessageHandler
{
private :
    using Parser = std::function<DataType(const std::string&)>;
    Parser _parser;

    using Listener = std::function<void(const DataType&)>;
    Listener _listener;

    std::shared_ptr<UartHandler> _uartHandler;

public :
    UartMessageHandler() = default;
    ~UartMessageHandler() = default;

public :
    auto initialize(std::shared_ptr<UartHandler> uartHandler,
                    Parser&& parser, 
                    Listener&& listener) -> void
    {
        if(_uartHandler || _parser || _listener) 
        { 
            throw std::runtime_error("Already initialized"); 
        }

        if(!uartHandler || !parser || !listener)
        {
            throw std::invalid_argument("All parameter must be set");
        }

        _uartHandler = std::move(uartHandler);
        _parser = std::move(parser);
        _listener = std::move(listener);
        _uartHandler->receiving([this](const auto& message){
            _listener(_parser(message));
        });
    }
};