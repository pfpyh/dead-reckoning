#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "uart/UartHandler.hpp"

#include <iostream>

class MockSerial : public ::common::Serial
{
public:
    MOCK_METHOD(bool, open, (const std::string&, const ::common::Baudrate, const uint8_t), (override, noexcept));
    MOCK_METHOD(void, close, (), (override, noexcept));
    MOCK_METHOD(bool, is_open, (), (override, noexcept));
    MOCK_METHOD(std::string, readline, (), (override, noexcept));
    MOCK_METHOD(bool, write, (const char*, const size_t), (override, noexcept));
};

TEST(test_UartHandler, singleHandlerReceiving)
{
    // given
    const std::string send{"Test message"};

    std::shared_ptr<MockSerial> serialMock = std::make_shared<MockSerial>();
    
    EXPECT_CALL(*serialMock, readline())
        .Times(::testing::AtLeast(1))
        .WillRepeatedly(::testing::Return(send));

    EXPECT_CALL(*serialMock, close()).Times(::testing::AtLeast(1));

    // when
    UartHandler handler(serialMock);
    auto threadFuture = handler.start();

    std::shared_ptr<std::promise<std::string>> promise 
        = std::make_shared<std::promise<std::string>>();

    handler.receiving([promise](const std::string& message) {
        static bool called = false;
        if(!called)
        {
            promise->set_value(message); 
            called = true;
        }
    });

    auto future = promise->get_future();
    auto status = future.wait_for(std::chrono::milliseconds(100));

    handler.stop();
    threadFuture.wait();

    // then
    ASSERT_EQ(status, std::future_status::ready);
    ASSERT_EQ(future.get(), send);
}

TEST(test_UartHandler, multipleHandlerReceiving)
{
    // given
    const std::string send{"Test message"};

    std::shared_ptr<MockSerial> serialMock = std::make_shared<MockSerial>();
    
    EXPECT_CALL(*serialMock, readline())
        .Times(::testing::AtLeast(1))
        .WillRepeatedly(::testing::Return(send));

    EXPECT_CALL(*serialMock, close()).Times(::testing::AtLeast(1));

    // when
    UartHandler handler(serialMock);
    auto threadFuture = handler.start();

    std::vector<std::shared_ptr<std::promise<std::string>>> promises;
    std::vector<std::shared_ptr<std::future<std::string>>> futures;

    for(uint8_t itor = 0; itor != 255; ++itor)
    {
        std::shared_ptr<std::promise<std::string>> promise 
            = std::make_shared<std::promise<std::string>>();
        
        auto flag = std::make_shared<std::once_flag>();
        handler.receiving([promise, itor, flag](const std::string& message) {
            std::call_once(*flag, [&]() {
                promise->set_value(message);
            });
        });

        promises.push_back(promise);
        futures.push_back(std::make_shared<std::future<std::string>>(promise->get_future()));
    }

    // then
    for(auto future : futures)
    {
        ASSERT_EQ(future->get(), send);
    }

    handler.stop();
    threadFuture.wait_for(std::chrono::milliseconds(100));
}

TEST(test_UartHandler, writing)
{
    // given
    std::shared_ptr<MockSerial> serialMock = std::make_shared<MockSerial>();

    EXPECT_CALL(*serialMock, write(::testing::_,  ::testing::_))
        .Times(2)
        .WillOnce(::testing::Return(false))
        .WillOnce(::testing::Return(true));

    EXPECT_CALL(*serialMock, close()).Times(::testing::AtLeast(1));

    // when
    UartHandler handler(serialMock);
    auto threadFuture = handler.start();

    bool first = handler.writing("Test Input");
    bool second = handler.writing("Test Input");

    handler.stop();
    threadFuture.wait();

    // then
    ASSERT_FALSE(first);
    ASSERT_TRUE(second);
}