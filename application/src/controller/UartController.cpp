#include "controller/UartController.hpp"

#include <fstream>
#include <sstream>
#include <algorithm>

#include "common/logging/Logger.hpp"

UartHolder::UartHolder(std::shared_ptr<::common::Serial> serial) noexcept
    : _serial(std::move(serial)) 
    , _onReceivedHandlers(new Handlers()) {}

UartHolder::~UartHolder() noexcept
{
    delete _onReceivedHandlers;
    _serial->close();
}

auto UartHolder::reading(std::function<void(const std::string&)>&& onReceivedHandler) -> void
{
    std::lock_guard<std::mutex> scopedLock(_lock);
    
    auto* previous = _onReceivedHandlers.load();
    auto* current = new Handlers(*previous);
    current->push_back(std::move(onReceivedHandler));

    _onReceivedHandlers.store(current);
    delete previous;
}

auto UartHolder::writing(const std::string& message) noexcept -> bool
{
    return _serial->write(message.c_str(), message.length());
}

auto UartHolder::__work() -> void
{
    auto recv(_serial->readline());
    if(!recv.empty())
    {
        auto handlers = _onReceivedHandlers.load();
        for(auto& handler : (*handlers)) { handler(recv); }
    }
}

UartController::UartController() = default;
UartController::~UartController() { finalize(); }

auto UartController::finalize() noexcept -> void
{
    for(uint8_t i = static_cast<uint8_t>(UartType::IMU); 
        i < static_cast<uint8_t>(UartType::MAX); 
        ++i)
    {
        auto holder = get(static_cast<UartType::type>(i));
        if(holder) { holder->stop(); }
    }
    for(auto& future : _futures) { future.wait(); }
    
    _holderMap.clear();
}

auto UartController::set(UartType::type type,
                         std::shared_ptr<::common::Serial> serial) noexcept -> void
{
    auto itor = _holderMap.find(type);
    if(itor != _holderMap.end()) 
    {
        _ERROR_("UartType(%d) is already registed", static_cast<int32_t>(type));
        std::terminate();
    }

    auto holder = std::make_shared<UartHolder>(serial);
    _futures.push_back(std::move(holder->start()));
    _holderMap.insert({type, holder});
}

auto UartController::get(UartType::type type) noexcept -> std::shared_ptr<UartHolder>
{
    auto itor = _holderMap.find(type);
    if(itor != _holderMap.end()) { return _holderMap[type]; }
    return nullptr;
}

auto UartController::read_configuration(const std::string& path) noexcept -> std::vector<UartConfiguration>
{
    std::vector<UartConfiguration> configurations;
    
    std::ifstream file(path);
    if (!file.is_open()) 
    {
        _ERROR_("Failed to open configuration file: %s", path.c_str());
        return configurations;
    }
    
    std::string line;
    while (std::getline(file, line)) 
    {
        if (line.empty() || std::all_of(line.begin(), line.end(), ::isspace)) { continue; }
        
        std::istringstream iss(line);
        std::string token;
        std::vector<std::string> tokens;
        
        while (std::getline(iss, token, ',')) 
        {
            token.erase(0, token.find_first_not_of(" 	"));
            token.erase(token.find_last_not_of(" 	") + 1);
            tokens.push_back(token);
        }
        
        if (tokens.size() != 4) 
        {
            _ERROR_("Invalid configuration line: %s (expected 4 comma-separated values)", line.c_str());
            continue;
        }
        
        UartType::type uartType;
        if (tokens[0] == "IMU") { uartType = UartType::IMU; } 
        else if (tokens[0] == "GNSS") { uartType = UartType::GNSS; } 
        else
        {
            _ERROR_("Unknown UART type: %s", tokens[0].c_str());
            continue;
        }
        
        std::string port = tokens[1];
        
        uint32_t baudrateValue;
        try 
        {
            baudrateValue = std::stoul(tokens[2]);
        } 
        catch (const std::exception& e) 
        {
            _ERROR_("Invalid baudrate value: %s", tokens[2].c_str());
            continue;
        }
        ::common::Baudrate baudrate(baudrateValue);
        
        uint8_t mode;
        try 
        {
            mode = static_cast<uint8_t>(std::stoul(tokens[3]));
        } 
        catch (const std::exception& e) 
        {
            _ERROR_("Invalid mode value: %s", tokens[3].c_str());
            continue;
        }

        UartConfiguration config;
        config._type = uartType;
        config._port = port;
        config._baudrate = baudrate;
        config._mode = mode;
        
        configurations.push_back(config);
        
        _INFO_("Loaded UART configuration: Type=%s, Port=%s, Baudrate=%u, Mode=%u",
               tokens[0].c_str(), port.c_str(), baudrateValue, mode);
    }
    
    file.close();
    return configurations;
}