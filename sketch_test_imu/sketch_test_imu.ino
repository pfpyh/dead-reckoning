/*
  라즈베리 파이 피코를 위한 가상 6축 IMU 센서 시뮬레이터

  이 코드는 실제 IMU 센서가 없어도 센서 데이터를 파싱하거나
  센서 퓨전 알고리즘을 테스트할 수 있도록 시뮬레이션된 데이터를 생성합니다.

  - 출력 주기: 50Hz (20ms 마다)
  - 출력 형식: "roll pitch yaw x y z" (공백으로 구분)
  - 데이터 특징: sin, cos 함수를 이용하여 부드럽게 회전하는 것처럼 값을 생성
*/

// sin, cos 함수를 사용하기 위해 math 라이브러리를 포함합니다.
#include <math.h>

// 시간의 흐름을 표현하기 위한 변수입니다. 이 값이 계속 증가하며 데이터에 변화를 줍니다.
float time_step = 0.0;

void setup() {
  // 50Hz의 빠른 데이터 전송을 위해 높은 통신 속도(115200)를 사용합니다.
  // 데이터를 수신하는 프로그램의 통신 속도도 반드시 115200으로 맞춰야 합니다.
  Serial.begin(115200);

  // 시리얼 포트가 준비될 때까지 잠시 대기
  while (!Serial) {
    ;
  }
  
  // 시뮬레이터 시작을 알리는 메시지를 한 번 출력
  Serial.println("IMU Simulator Started. Outputting data at 50Hz.");
  Serial.println("Format: roll pitch yaw x y z");
  Serial.println("----------------------------------------");
  delay(1000); // 메시지를 읽을 시간을 줍니다.
}

void loop() {
  // --- 1. 시뮬레이션 값 계산 ---
  // sin, cos 함수를 사용하여 주기적이고 부드럽게 변하는 값을 만듭니다.
  // 값의 범위를 조절하려면 앞의 상수를, 회전 속도를 조절하려면 time_step에 곱해지는 상수를 변경하세요.
  
  // 각도(Orientation) 데이터 시뮬레이션 (단위: degrees)
  float roll  = 45.0 * sin(time_step * 0.5);
  float pitch = 30.0 * cos(time_step * 0.7);
  float yaw   = 90.0 * sin(time_step * 0.3);

  // 가속도(Acceleration) 데이터 시뮬레이션 (단위: m/s^2)
  // z축은 중력가속도(9.8)를 기본값으로 약간의 변화를 줍니다.
  float acc_x = 2.0 * sin(time_step * 1.5);
  float acc_y = 2.0 * cos(time_step * 1.1);
  float acc_z = 9.8 + 1.0 * sin(time_step);


  // --- 2. 지정된 형식으로 데이터 출력 ---
  // 각 값을 공백으로 구분하여 한 줄에 출력합니다.
  Serial.print(acc_x);
  Serial.print(" ");
  Serial.print(acc_y);
  Serial.print(" ");
  Serial.print(acc_z);
  Serial.print(" ");
  Serial.print(roll);
  Serial.print(" ");
  Serial.print(pitch);
  Serial.print(" ");
  Serial.println(yaw);


  // --- 3. 50Hz 주기 맞추기 ---
  // 1초(1000ms) / 50Hz = 20ms. 20ms 마다 루프가 실행되도록 딜레이를 줍니다.
  delay(20);

  // 다음 계산을 위해 시간 변수를 조금 증가시킵니다.
  // 이 값을 키우면 전체적인 회전 속도가 빨라집니다.
  time_step += 0.01;
}