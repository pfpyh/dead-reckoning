#pragma once

#include <vector>
#include <functional>
#include <atomic>
#include <unordered_map>
#include <future>
#include <string>

#include "common/NonCopyable.hpp"
#include "common/thread/Runnable.hpp"
#include "common/communication/Serial.hpp"

struct UartType
{
    enum type : uint8_t
    {
        IMU = 0,
        GNSS,
        MAX,
    };
};

struct UartConfiguration
{
    UartType::type _type;
    std::string _port;
    ::common::Baudrate _baudrate;
    uint8_t _mode;
};

class UartHolder final : public ::common::Runnable
{
private :
    using Handlers = std::vector<std::function<void(const std::string&)>>;

    std::mutex _lock;
    std::atomic<Handlers*> _onReceivedHandlers;

    std::shared_ptr<::common::Serial> _serial;
    std::shared_ptr<std::future<void>> _future;

public :
    explicit UartHolder(std::shared_ptr<::common::Serial> serial) noexcept;
    ~UartHolder() noexcept;

public :
    auto reading(std::function<void(const std::string&)>&& onReceivedHandler) -> void;
    auto writing(const std::string& message) noexcept -> bool;

private :
    auto __work() -> void override;
};

class UartController : public ::common::NonCopyable
{
private :
    std::unordered_map<UartType::type, std::shared_ptr<UartHolder>> _holderMap;
    std::vector<std::future<void>> _futures;

public :
    UartController();
    ~UartController();

public :
    auto finalize() noexcept -> void;

    auto set(UartType::type type, 
             std::shared_ptr<::common::Serial> serial) noexcept -> void;
    auto get(UartType::type type) noexcept -> std::shared_ptr<UartHolder>;

public :
    static auto read_configuration(const std::string& path) noexcept -> std::vector<UartConfiguration>;
};