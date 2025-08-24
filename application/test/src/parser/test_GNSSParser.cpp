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
    GNSSParser parser;
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
    GNSSParser parser;
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
    GNSSParser parser;
    const auto gnss = parser.parse(ubx);

    // then
    ASSERT_EQ(gnss._type, MessageType::UBX);
    // ASSERT_EQ(gnss._message, ubx);
    // ASSERT_EQ(parser.buffer_length(), 2);
}

TEST(test_GNSSParser, complex)
{
    // given
    unsigned char nmea_test_data[] = {
        // 깨진 문자 (임의의 비-ASCII 바이트)
        0xBB, 0xBF, // \xb6\xba 대신 임의의 0xBB, 0xBF 사용

        // GbA*70\r\n
        0x47, 0x62, 0x41, 0x2A, 0x37, 0x30, 0x0D, 0x0A, 

        // $GNRMC,133122.00,A,3733.30733,N,12644.82286,E,0.159,,221025,,,A*66\r\n
        0x24, 0x47, 0x4E, 0x52, 0x4D, 0x43, 0x2C, 0x31, 0x33, 0x33, 0x31, 0x32, 0x32, 0x2E, 0x30, 0x30,
        0x2C, 0x41, 0x2C, 0x33, 0x37, 0x33, 0x33, 0x2E, 0x33, 0x30, 0x37, 0x33, 0x33, 0x2C, 0x4E, 0x2C,
        0x31, 0x32, 0x36, 0x34, 0x34, 0x2E, 0x38, 0x32, 0x32, 0x38, 0x36, 0x2C, 0x45, 0x2C, 0x30, 0x2E,
        0x31, 0x35, 0x39, 0x2C, 0x2C, 0x32, 0x32, 0x31, 0x30, 0x32, 0x35, 0x2C, 0x2C, 0x2C, 0x41, 0x2A,
        0x36, 0x36, 0x0D, 0x0A, // \r\n

        // $GNVTG,,T,,M,0.159,N,0.294,K,A*3F\r\n
        0x24, 0x47, 0x4E, 0x56, 0x54, 0x47, 0x2C, 0x2C, 0x54, 0x2C, 0x2C, 0x4D, 0x2C, 0x30, 0x2E, 0x31,
        0x35, 0x39, 0x2C, 0x4E, 0x2C, 0x30, 0x2E, 0x32, 0x39, 0x34, 0x2C, 0x4B, 0x2C, 0x41, 0x2A, 0x33,
        0x46, 0x0D, 0x0A // \r\n
    };

    GNSSParser parser;
    std::string test_data(reinterpret_cast<const char*>(nmea_test_data), sizeof(nmea_test_data));
    const auto gnss = parser.parse(test_data);

    ASSERT_EQ(gnss._type, MessageType::NMEA);
}



