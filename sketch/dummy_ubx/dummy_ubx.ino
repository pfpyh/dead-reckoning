// u-blox NAV-PVT 메시지 구조체 (92 bytes payload)
struct NAV_PVT_Message {
  uint32_t iTOW;      // GPS time of week of navigation epoch (ms)
  uint16_t year;      // Year (UTC)
  uint8_t  month;     // Month, range 1..12 (UTC)
  uint8_t  day;       // Day of month, range 1..31 (UTC)
  uint8_t  hour;      // Hour of day, range 0..23 (UTC)
  uint8_t  min;       // Minute of hour, range 0..59 (UTC)
  uint8_t  sec;       // Seconds of minute, range 0..60 (UTC)
  uint8_t  valid;     // Validity Flags (e.g., 0x03 = valid date and time)
  uint32_t tAcc;      // Time accuracy estimate (ns)
  int32_t  nano;      // Fraction of second, range -1e9..1e9 (ns)
  uint8_t  fixType;   // GNSSfix Type: 0..5
  uint8_t  flags;     // Fix Status Flags
  uint8_t  flags2;    // Additional flags
  uint8_t  numSV;     // Number of satellites used in Nav Solution
  int32_t  lon;       // Longitude (deg * 1e-7)
  int32_t  lat;       // Latitude (deg * 1e-7)
  int32_t  height;    // Height above Ellipsoid (mm)
  int32_t  hMSL;      // Height above mean sea level (mm)
  uint32_t hAcc;      // Horizontal accuracy estimate (mm)
  uint32_t vAcc;      // Vertical accuracy estimate (mm)
  int32_t  velN;      // NED north velocity (mm/s)
  int32_t  velE;      // NED east velocity (mm/s)
  int32_t  velD;      // NED down velocity (mm/s)
  int32_t  gSpeed;    // Ground Speed (2-D) (mm/s)
  int32_t  headMot;   // Heading of motion (2-D) (deg * 1e-5)
  uint32_t sAcc;      // Speed accuracy estimate (mm/s)
  uint32_t headAcc;   // Heading accuracy estimate (deg * 1e-5)
  uint16_t pDOP;      // Position DOP
  uint8_t  reserved1[6];
  int32_t  headVeh;   // Heading of vehicle (2-D) (deg * 1e-5)
  int16_t  magDec;    // Magnetic declination (deg * 1e-2)
  uint16_t magAcc;    // Magnetic declination accuracy (deg * 1e-2)
};

// UBX 프로토콜 헤더
const uint8_t UBX_HEADER[] = { 0xB5, 0x62 };
// NAV-PVT 메시지 ID
const uint8_t UBX_NAV_PVT_ID[] = { 0x01, 0x07 };

unsigned long previousMillis = 0;
const long interval = 1000; // 1Hz

NAV_PVT_Message pvt;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ; // 시리얼 포트가 연결될 때까지 대기
  }

  // 구조체에 기본값 채우기 (시뮬레이션용)
  pvt.iTOW = 350000000;
  pvt.year = 2025;
  pvt.month = 8;
  pvt.day = 24;
  pvt.hour = 7;
  pvt.min = 30;
  pvt.sec = 0;
  pvt.valid = 0x07; // 유효한 시간, 날짜, 완전히 해결됨
  pvt.tAcc = 30;
  pvt.nano = 0;
  pvt.fixType = 3; // 3D fix
  pvt.flags = 0x01; // gnssFixOK
  pvt.numSV = 12;
  // 초기 좌표: 서울 근처 (단위: 1e-7 deg)
  pvt.lon = 1269780000;
  pvt.lat = 375665000;
  pvt.height = 50000; // 50m
  pvt.hMSL = 60000;   // 60m
  pvt.hAcc = 1500;    // 1.5m
  pvt.vAcc = 3000;    // 3.0m
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // 동적 데이터 업데이트
    updatePVTData();

    // UBX 메시지 전송
    sendUBXMessage((uint8_t*)&pvt, sizeof(pvt));
  }
}

void updatePVTData() {
  // 매초 시간 및 좌표 값을 조금씩 변경하여 움직이는 것처럼 시뮬레이션
  pvt.iTOW += 1000; // 1초 증가
  pvt.sec++;
  if (pvt.sec >= 60) {
    pvt.sec = 0;
    pvt.min++;
  }
  // 위도/경도를 약간씩 변경 (예: 북동쪽으로 이동)
  pvt.lat += 50; // 약 0.5cm 이동
  pvt.lon += 40; // 약 0.4cm 이동
}

void sendUBXMessage(uint8_t* payload, uint16_t length) {
  // 1. 헤더 전송
  Serial.write(UBX_HEADER, sizeof(UBX_HEADER));
  
  // 2. 클래스 및 ID 전송
  Serial.write(UBX_NAV_PVT_ID, sizeof(UBX_NAV_PVT_ID));

  // 3. 길이(Length) 전송 (Little Endian)
  Serial.write((uint8_t)(length & 0xFF));
  Serial.write((uint8_t)(length >> 8));

  // 4. 페이로드(Payload) 전송
  Serial.write(payload, length);
  
  // 5. 체크섬 계산 및 전송
  uint8_t ck_a = 0, ck_b = 0;
  // 클래스, ID, 길이, 페이로드에 대해 체크섬 계산
  for (int i = 0; i < sizeof(UBX_NAV_PVT_ID); i++) {
    ck_a += UBX_NAV_PVT_ID[i];
    ck_b += ck_a;
  }
  ck_a += (uint8_t)(length & 0xFF);
  ck_b += ck_a;
  ck_a += (uint8_t)(length >> 8);
  ck_b += ck_a;
  for (int i = 0; i < length; i++) {
    ck_a += payload[i];
    ck_b += ck_a;
  }
  
  Serial.write(ck_a);
  Serial.write(ck_b);
}