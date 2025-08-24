#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "uart/UartMessageHandler.hpp"

class MockSerial : public ::common::Serial
{
public:
    MOCK_METHOD(bool, open, (const std::string&, const ::common::Baudrate, const uint8_t), (override, noexcept));
    MOCK_METHOD(void, close, (), (override, noexcept));
    MOCK_METHOD(bool, is_open, (), (override, noexcept));
    MOCK_METHOD(bool, read, (char*, size_t), (override, noexcept));
    MOCK_METHOD(std::string, readline, (::common::EscapeSequence::type), (override, noexcept));
    MOCK_METHOD(bool, write, (const char*, const size_t), (override, noexcept));
};

TEST(test_UartMessageHandler, singleHandler)
{
    // given
    const std::string send{"Test message"};

    std::shared_ptr<MockSerial> serialMock = std::make_shared<MockSerial>();
    
    EXPECT_CALL(*serialMock, readline(::testing::_))
        .Times(::testing::AtLeast(1))
        .WillRepeatedly(::testing::Return(send));

    EXPECT_CALL(*serialMock, close()).Times(::testing::AtLeast(1));

    std::shared_ptr<UartHandler> uarthandler =
        std::make_shared<UartHandler>(serialMock);
    auto uartFuture = uarthandler->start();

    // when
    struct Data
    {
        int32_t _1 = 0;
        int32_t _2 = 0;
    };

    std::shared_ptr<std::promise<void>> promise =
        std::make_shared<std::promise<void>>();
    auto future = promise->get_future();
    Data receivedData;

    UartMessageHandler<Data> msgHandler;
    msgHandler.initialize(uarthandler,
                          [](const std::string& message) -> Data {
        Data data;
        data._1 = 1;
        data._2 = 1;
        return data;
    }, [&receivedData, &promise](const Data& data) {
        static bool called = false;
        if(!called)
        {
            receivedData._1 = data._1;
            receivedData._2 = data._2;

            called = true;
            promise->set_value();
        }
    });

    future.wait();
    uarthandler->stop();
    uartFuture.wait();

    // then
    ASSERT_EQ(receivedData._1, 1);
    ASSERT_EQ(receivedData._2, 1);
}