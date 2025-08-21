#pragma once

#include "common/thread/Runnable.hpp"
#include "common/thread/TaskExecutor.hpp"
#include "common/communication/Serial.hpp"

#include <array>

struct UartType
{
    enum type : uint8_t
    {
        IMU = 0,
        GNSS,
        MAX,
    };
};

class UartHandler final : public ::common::Runnable
{
private :
    using onReceiveListener = std::function<void(const std::string&)>;

    // Todo: Need to change when common-lib support DoublingBuffer [COMMO-22]
    class ListenerBuffer final
    {
    private :
        std::atomic<uint32_t> _generation{0};
        std::atomic<uint8_t> _activatedIndex{0};
        std::array<std::shared_ptr<std::vector<onReceiveListener>>, 2> _buffers;

    public :
        ListenerBuffer()
        {
            _buffers[0] = std::make_shared<std::vector<onReceiveListener>>();
        }

        auto add(onReceiveListener&& listener) -> void
        {
            uint8_t selectedIndex = _activatedIndex.load() == 0 ? 1 : 0;
            _buffers[selectedIndex] = std::make_shared<std::vector<onReceiveListener>>((*_buffers[_activatedIndex.load()]));
            _buffers[selectedIndex]->push_back(std::move(listener));
            _activatedIndex.store(selectedIndex);
        }

        auto get_buffer() -> std::shared_ptr<std::vector<onReceiveListener>>
        {
            return _buffers[_activatedIndex.load()];
        }
    };

private :
    ListenerBuffer _listenerBuffer;

    std::shared_ptr<::common::Serial> _serial;
    std::shared_ptr<std::future<void>> _future;

    static std::shared_ptr<::common::TaskExecutor> _executor;

public :
    explicit UartHandler(std::shared_ptr<::common::Serial> serial) noexcept;
    ~UartHandler() noexcept;

public :
    auto receiving(std::function<void(const std::string&)>&& onReceiveListener) -> void;
    auto writing(const std::string& message) noexcept -> bool;

private :
    auto __work() -> void override;
};