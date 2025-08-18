#include"LIB.h"

void setup() {
  Serial.begin(9600);
  Oled_Init();
  Button_Init();
  MPU6050_Init(0x68, G1000DPS, A16G);
  displayHomeScreen();
  delay(2000);
  
}

void loop() {
  Button_Loop();
  Enable_MPU();

}

