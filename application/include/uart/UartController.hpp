#pragma once

#include "uart/UartHandler.hpp"

#include <vector>
#include <functional>
#include <atomic>
#include <unordered_map>
#include <future>
#include <string>

#include "common/NonCopyable.hpp"
#include "common/thread/Runnable.hpp"
#include "common/communication/Serial.hpp"

struct UartConfiguration
{
    UartType::type _type;
    std::string _port;
    ::common::Baudrate _baudrate;
    uint8_t _mode;
};

class UartController : public ::common::NonCopyable
{
private :
    std::unordered_map<UartType::type, std::shared_ptr<UartHandler>> _holderMap;
    std::vector<std::future<void>> _futures;

public :
    UartController();
    ~UartController();

public :
    auto finalize() noexcept -> void;

    auto set(UartType::type type, 
             std::shared_ptr<::common::Serial> serial) noexcept -> void;
    auto get(UartType::type type) noexcept -> std::shared_ptr<UartHandler>;

public :
    static auto read_configuration(const std::string& path) noexcept -> std::vector<UartConfiguration>;
};