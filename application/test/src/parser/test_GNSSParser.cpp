#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "parser/GNSSParser.hpp"

#include "common/logging/Logger.hpp"

TEST(test_GNSSParser, NMEA)
{
    // given
    const std::string GLGSV{"$GLGSV,3,2,10,77,05,021,,78,36,071,,79,30,135,,83,23,204,*69"};
    const std::string input = GLGSV + "\r\n";

    // when
    UBXParser parser;
    const auto gnss = parser.parse(input);

    // then
    ASSERT_EQ(gnss._type, MessageType::NMEA);
    ASSERT_EQ(gnss._message, GLGSV);
}

TEST(test_GNSSParser, NMEAwithCorruptedMsg)
{
    // given
    const std::string GLGSV{"$GLGSV,3,2,10,77,05,021,,78,36,071,,79,30,135,,83,23,204,*69"};
    const std::string input = "GSV,3,3,1\r\n" + GLGSV + "\r\n";

    // when
    UBXParser parser;
    const auto gnss = parser.parse(input);

    // then
    ASSERT_EQ(gnss._type, MessageType::NMEA);
    ASSERT_EQ(gnss._message, GLGSV);
    ASSERT_EQ(parser.buffer_length(), 2);
}

TEST(test_GNSSParser, UBX_RXM_TM)
{
    // given
    const std::string ubx{"킶  4"};

    // when
    UBXParser parser;
    const auto gnss = parser.parse(ubx);

    // then
    ASSERT_EQ(gnss._type, MessageType::UBX);
    // ASSERT_EQ(gnss._message, ubx);
    // ASSERT_EQ(parser.buffer_length(), 2);
}

TEST(test_GNSSParser, complex)
{
    // given
    const std::string message{"$GPGSV,3,3,10,23,46,318,,24,65,183,*7E킶  4$GLGSV,3,1,10,67,02,079,,68,39,047,,69,50,331,,70,04,280,*6E"};
}



