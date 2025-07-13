#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "imu/Imu.hpp"

#include "mock/mock_Serial.hpp"

namespace engine::test
{
TEST(test_Imu, common_test)
{
    // given
    auto mockSerial = std::make_shared<mock::MockSerial>();
    EXPECT_CALL(*mockSerial, readline())
        .WillRepeatedly(testing::Return("100 -100 100 -100 100 -100"));

    auto imu = Imu::create(mockSerial);

    // when
    class Receiver : public ::common::Observer<Receiver, ImuData>
    {
    public :
        ImuData _last;
        bool _received = false;
        std::weak_ptr<Imu> _imu;

    public :
        Receiver(decltype(_imu) imu)
            : _imu(imu) {}

    public :
        auto onEvent(ImuData data) -> void override
        {
            _last = data;
            _received = true;
            _imu.lock()->stop();
        }
    };

    auto receiver = Receiver::create(imu);
    imu->subscribe_update_imuData(receiver);
    auto future = imu->start();

    // then
    future.wait();
    ASSERT_TRUE(receiver->_received);
}
} // namespace engine::test