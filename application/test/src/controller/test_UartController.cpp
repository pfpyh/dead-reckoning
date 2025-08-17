#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <thread>

#include "controller/UartController.hpp"

class MockSerial : public ::common::Serial
{
public:
    MOCK_METHOD(bool, open, (const std::string&, const ::common::Baudrate, const uint8_t), (override, noexcept));
    MOCK_METHOD(void, close, (), (override, noexcept));
    MOCK_METHOD(bool, is_open, (), (override, noexcept));
    MOCK_METHOD(std::string, readline, (), (override, noexcept));
    MOCK_METHOD(bool, write, (const char*, const size_t), (override, noexcept));
};

TEST(test_UartHolder, reading)
{
    // given
    const std::string send{"Test message"};

    std::shared_ptr<MockSerial> serialMock = std::make_shared<MockSerial>();
    
    EXPECT_CALL(*serialMock, readline())
        .Times(::testing::AtLeast(1))
        .WillRepeatedly(::testing::Return(send));

    EXPECT_CALL(*serialMock, close()).Times(::testing::AtLeast(1));

    UartHolder holder(serialMock);
    auto threadFuture = holder.start();

    // when
    std::shared_ptr<std::promise<std::string>> promise 
        = std::make_shared<std::promise<std::string>>();

    bool once = true;
    holder.reading([promise, &once](const std::string& message) {
        if(!once) { return; }

        promise->set_value(message); 
        once = false;
    });

    auto future = promise->get_future();
    auto status = future.wait_for(std::chrono::milliseconds(100));

    holder.stop();
    threadFuture.wait();

    // then
    ASSERT_EQ(status, std::future_status::ready);
    ASSERT_EQ(future.get(), send);
}

TEST(test_UartHolder, writing)
{
    // given
    std::shared_ptr<MockSerial> serialMock = std::make_shared<MockSerial>();

    EXPECT_CALL(*serialMock, write(::testing::_,  ::testing::_))
        .Times(2)
        .WillOnce(::testing::Return(false))
        .WillOnce(::testing::Return(true));

    EXPECT_CALL(*serialMock, close()).Times(::testing::AtLeast(1));

    UartHolder holder(serialMock);
    auto threadFuture = holder.start();

    // when
    bool first = holder.writing("Test Input");
    bool second = holder.writing("Test Input");

    holder.stop();
    threadFuture.wait();

    // then
    ASSERT_FALSE(first);
    ASSERT_TRUE(second);
}

TEST(test_UartController, setUartAndReading)
{
    // given
    const std::string send{"Test message"};

    std::shared_ptr<MockSerial> serialMock = std::make_shared<MockSerial>();

    EXPECT_CALL(*serialMock, readline())
        .Times(::testing::AtLeast(1))
        .WillRepeatedly(::testing::Return(send));

    EXPECT_CALL(*serialMock, close()).Times(::testing::AtLeast(1));

    // when
    UartController controller;
    controller.set(UartType::IMU, serialMock);

    std::shared_ptr<std::promise<std::string>> promise 
        = std::make_shared<std::promise<std::string>>();
    auto holder = controller.get(UartType::IMU);
    bool once = true;
    holder->reading([promise, &once](const std::string& message) {
        if(!once) { return; }

        promise->set_value(message); 
        once = false;
    });

    auto future = promise->get_future();
    auto status = future.wait_for(std::chrono::milliseconds(100));

    // then
    ASSERT_EQ(status, std::future_status::ready);
    ASSERT_EQ(future.get(), send);
}

TEST(test_UartContoller, tryGetWithInvalidType)
{
    // given
    const std::string send{"Test message"};

    std::shared_ptr<MockSerial> serialMock = std::make_shared<MockSerial>();

    EXPECT_CALL(*serialMock, readline())
        .WillRepeatedly(::testing::Return(send));

    EXPECT_CALL(*serialMock, close()).Times(::testing::AtLeast(1));

    UartController controller;
    controller.set(UartType::IMU, serialMock);

    // when & then
    ASSERT_TRUE(controller.get(UartType::IMU) != nullptr);
    ASSERT_TRUE(controller.get(UartType::MAX) == nullptr);
}

TEST(test_UartController, duplicatedUartType)
{
    // given
    const std::string send{"Test message"};

    std::shared_ptr<MockSerial> serialMock = std::make_shared<MockSerial>();

    EXPECT_CALL(*serialMock, readline())
        .Times(::testing::AtLeast(1))
        .WillRepeatedly(::testing::Return(send));

    EXPECT_CALL(*serialMock, close()).Times(::testing::AtLeast(1));

    UartController controller;
    controller.set(UartType::IMU, serialMock);

    // when & then
    EXPECT_DEATH({
        controller.set(UartType::IMU, nullptr);
    }, ".*");
}