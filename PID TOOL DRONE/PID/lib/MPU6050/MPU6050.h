#ifndef MPU_H
#define MPU_H

#include "Arduino.h"
#include "Wire.h"
#include "BUTTON.h"
//#include "OneButton.h"
#include "ESP32BUTTON.h"
#include "DELAY.h"
#include "OLED.h"


extern uint8_t Load_percent;

// ========== I2C Address ==========
#define MPU_I2C_ADDRESS     0x68 // mặc định

// ========== Register Map ==========
#define SMPRT_DIV           0x19  // Sample Rate Divider
#define CONFIG              0x1A  // DLPF config
#define GYRO_CONFIG         0x1B  // Gyro full-scale
#define ACCEL_CONFIG        0x1C  // Accel full-scale

#define ACCEL_XOUT_H        0x3B
#define ACCEL_XOUT_L        0x3C
#define ACCEL_YOUT_H        0x3D
#define ACCEL_YOUT_L        0x3E
#define ACCEL_ZOUT_H        0x3F
#define ACCEL_ZOUT_L        0x40

#define TEMP_OUT_H          0x41
#define TEMP_OUT_L          0x42

#define GYRO_XOUT_H         0x43
#define GYRO_XOUT_L         0x44
#define GYRO_YOUT_H         0x45
#define GYRO_YOUT_L         0x46
#define GYRO_ZOUT_H         0x47
#define GYRO_ZOUT_L         0x48

#define PWR_MGMT_1          0x6B

// ========== FS Ranges ==========
enum ACCEL_FS {
    A2G  = 0,  // ±2g
    A4G  = 1,  // ±4g
    A8G  = 2,  // ±8g
    A16G = 3   // ±16g
};

enum GYRO_FS {
    G250DPS  = 0, // ±250 °/s
    G500DPS  = 1, // ±500 °/s
    G1000DPS = 2, // ±1000 °/s
    G2000DPS = 3  // ±2000 °/s
};


///////////////////////////////////////////////////////////////Khai báo extern varible output//////////////////////////////////////////////////
extern float pitch, roll, yaw;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ========== Public API ==========
// Giữ signature cũ để tương thích:
// i2c_address: 0x68 hoặc 0x69
// fs_sel: GYRO_FS
// afs_sel: ACCEL_FS
extern void MPU6050_Init(uint8_t i2c_address, uint8_t fs_sel, uint8_t afs_sel);

// Đọc chỉ Gyro 
extern void ReadGyroscope(void);

// NEW: Đọc chỉ Accel (nếu cần)
extern void ReadAccelerometer(void);

extern void ReadAll(void);

// Hiệu chuẩn bias gyro
extern void CalibrateGyro(uint16_t samples, uint16_t wait_ms);

// Cập nhật góc từ GYRO (tích phân)
extern void GyroAngleFromGyro(void);

extern void FusedAngle_Update(float alpha = 0.98f);

extern void Enable_MPU(void);
// In debug góc Pitch/Roll/Yaw
extern void Debug_MPU(void);

float MPU_GetRoll(void);
float MPU_GetPitch(void);
float MPU_GetYaw(void);


int16_t MPU_GetRawGX(void);
int16_t MPU_GetRawGY(void);
int16_t MPU_GetRawGZ(void);
int16_t MPU_GetRawAX(void);
int16_t MPU_GetRawAY(void);
int16_t MPU_GetRawAZ(void);

#endif // MPU_H
