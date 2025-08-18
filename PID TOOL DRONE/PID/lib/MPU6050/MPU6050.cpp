#include "MPU6050.h"

// ================== Internal State ==================
struct MPU6050_State {
  // Raw
  int16_t ax = 0, ay = 0, az = 0;
  int16_t gx = 0, gy = 0, gz = 0;

  // Angle (deg)
  float roll = 0.0f, pitch = 0.0f, yaw = 0.0f;

  // Bias (LSB)
  float gyroBiasX = 0.0f, gyroBiasY = 0.0f, gyroBiasZ = 0.0f;

  // Sensitivity (LSB per unit)
  float gyroSens = 131.0f;     // LSB/°/s
  float accelSens = 16384.0f;  // LSB/g

  // Timekeeping
  uint32_t lastMicros = 0;

  // I2C addr
  uint8_t addr = MPU_I2C_ADDRESS;

  // Check yaw calirate 
  volatile bool zeroYawPending;
};

static MPU6050_State mpu;

// Sensitivity tables
static const float kGyroSensTable[4]  = {131.0f, 65.5f, 32.8f, 16.4f};
static const float kAccelSensTable[4] = {16384.0f, 8192.0f, 4096.0f, 2048.0f};

uint8_t Load_percent = 0;

// Output varible
float pitch = 0.0, roll = 0.0, yaw = 0.0;

// ================== Low-level helpers ==================
static inline void writeReg(uint8_t reg, uint8_t data) {
  Wire.beginTransmission(mpu.addr);
  Wire.write(reg);
  Wire.write(data);
  Wire.endTransmission();
}

static inline void readBurst(uint8_t startReg, uint8_t count) {
  Wire.beginTransmission(mpu.addr);
  Wire.write(startReg);
  Wire.endTransmission(false);
  Wire.requestFrom((int)mpu.addr, (int)count, (int)true);
}

// ================== Public API ==================
void MPU6050_Init(uint8_t i2c_address, uint8_t fs_sel, uint8_t afs_sel) {
  mpu.addr = i2c_address;

  Wire.begin();
  // Wake up, set clock to internal
  writeReg(PWR_MGMT_1, 0x00);

  // DLPF = 4 
  writeReg(CONFIG, 1);

  // Set FS ranges 
  writeReg(GYRO_CONFIG,  (uint8_t)(fs_sel  << 3));
  writeReg(ACCEL_CONFIG, (uint8_t)(afs_sel << 3));

  // Sample Rate Divider = 1 
  writeReg(SMPRT_DIV, 0x01);

  // Sensitivity
  mpu.gyroSens  = kGyroSensTable[fs_sel & 0x03];
  mpu.accelSens = kAccelSensTable[afs_sel & 0x03];

  // Reset time base
  mpu.lastMicros = micros();
}

// Gyro 
void ReadGyroscope(void) {
  readBurst(GYRO_XOUT_H, 6);
  mpu.gx = (int16_t)((Wire.read() << 8) | Wire.read());
  mpu.gy = (int16_t)((Wire.read() << 8) | Wire.read());
  mpu.gz = (int16_t)((Wire.read() << 8) | Wire.read());
}

// NEW: Đọc chỉ Accel
void ReadAccelerometer(void) {
  readBurst(ACCEL_XOUT_H, 6);
  mpu.ax = (int16_t)((Wire.read() << 8) | Wire.read());
  mpu.ay = (int16_t)((Wire.read() << 8) | Wire.read());
  mpu.az = (int16_t)((Wire.read() << 8) | Wire.read());
}

// NEW: Đọc cả Accel + Gyro (bỏ qua Temp) trong 1 lần
void ReadAll(void) {
  readBurst(ACCEL_XOUT_H, 14);
  mpu.ax = (int16_t)((Wire.read() << 8) | Wire.read());
  mpu.ay = (int16_t)((Wire.read() << 8) | Wire.read());
  mpu.az = (int16_t)((Wire.read() << 8) | Wire.read());
  // Bỏ qua nhiệt độ
  Wire.read(); Wire.read();
  mpu.gx = (int16_t)((Wire.read() << 8) | Wire.read());
  mpu.gy = (int16_t)((Wire.read() << 8) | Wire.read());
  mpu.gz = (int16_t)((Wire.read() << 8) | Wire.read());
}



// Hiệu chuẩn bias gyro (để yên cảm biến) – NON-BLOCKING, gọi lặp lại trong loop
void CalibrateGyro(uint16_t samples, uint16_t wait_ms) {
  // Trạng thái tích lũy qua các lần gọi
  static bool     inited = false;
  static uint16_t total  = 0;
  static uint16_t count  = 0;
  static uint16_t period = 1;
  static long sx = 0, sy = 0, sz = 0;

  // Khởi tạo lần đầu hoặc khi đổi cấu hình
  if (!inited || total != samples || period != wait_ms) {
    inited = true;
    total  = (samples == 0) ? 1 : samples;
    period = (wait_ms == 0) ? 1 : wait_ms;
    count  = 0;
    sx = sy = sz = 0;
    // Bạn có thể reset phần trăm ở đây nếu cần
    Load_percent = 0;        // <-- dùng biến của bạn
  }

  // Lấy 1 mẫu mỗi period ms
  if (DelayMillis(period)) {
    ReadGyroscope();         // đọc 1 lần
    sx += mpu.gx;
    sy += mpu.gy;
    sz += mpu.gz;
    count++;

    // Cập nhật %
    if (count > total) count = total;
    Load_percent = (uint8_t)((count * 100UL) / total);
  }

  // Xong đủ mẫu -> tính bias & thoát chế độ cali
  if (count >= total) {
    mpu.gyroBiasX = (float)sx / (float)total;
    mpu.gyroBiasY = (float)sy / (float)total;
    mpu.gyroBiasZ = (float)sz / (float)total;

    // kết thúc calib
    cali_check = false;   // bạn đang dùng biến này để biết còn đang cali không
    mpu_check  = true;    // bật đọc/gộp góc

    // chuẩn bị cho lần sau (không bắt buộc)
    inited = false;
    // Cho biến check cali hoàn thành thì set yaw ở góc đó bằng 0
    // Gọi hàm này ngay sau khi CalibrateGyro(...) xong
    mpu.zeroYawPending = true;
  }
}

// NEW: Lọc bù (Gyro + Accel) cho roll/pitch – góc ổn định hơn
void FusedAngle_Update(float alpha) {
  // Clamp alpha
  if (alpha < 0.0f) alpha = 0.0f;
  if (alpha > 1.0f) alpha = 1.0f;

  ReadAll();

  uint32_t now = micros();
  if (mpu.lastMicros == 0) { mpu.lastMicros = now; return; }
  float dt = (now - mpu.lastMicros) * 1e-6f;
  mpu.lastMicros = now;
  if (dt > 0.1f) dt = 0.1f;

  // Gyro -> dps
  float gx_dps = ( (float)mpu.gx - mpu.gyroBiasX ) / mpu.gyroSens;
  float gy_dps = ( (float)mpu.gy - mpu.gyroBiasY ) / mpu.gyroSens;
  float gz_dps = ( (float)mpu.gz - mpu.gyroBiasZ ) / mpu.gyroSens;

  // Tích phân gyro
  float roll_gyro  = mpu.roll  + gx_dps * dt;
  float pitch_gyro = mpu.pitch + gy_dps * dt;
  mpu.yaw         += gz_dps * dt; // yaw không có accel hỗ trợ

  // Accel -> góc tuyệt đối (deg)
  // Lưu ý: ax, ay, az là LSB. Đổi sang g không cần thiết cho atan2.
  const float ax = (float)mpu.ax;
  const float ay = (float)mpu.ay;
  const float az = (float)mpu.az;

  float accelRoll  = atan2f(ay, az) * 180.0f / PI;
  float accelPitch = atan2f(-ax, sqrtf(ay*ay + az*az)) * 180.0f / PI;

  // Complementary filter
  mpu.roll  = alpha * roll_gyro  + (1.0f - alpha) * accelRoll;
  mpu.pitch = alpha * pitch_gyro + (1.0f - alpha) * accelPitch;

  // Nếu vừa calibrate xong và muốn set yaw = 0, làm đúng một lần
  if (mpu.zeroYawPending) {
    mpu.yaw = 0.0f;
    mpu.zeroYawPending = false;
  }
  // (tuỳ chọn) Chuẩn hoá góc về [-180, 180]
  if (mpu.yaw > 180.0f)  mpu.yaw -= 360.0f;
  if (mpu.yaw < -180.0f) mpu.yaw += 360.0f;
  // Output
  pitch    = mpu.pitch;
  roll     = mpu.roll;
  yaw      = mpu.yaw;
}

void Enable_MPU(void){
  if (cali_check){
    CalibrateGyro(1000, 2);
    displayCaliLoading(Load_percent, cali_check);
    if (!cali_check){
      currentMenu = TOOL_MENU;
      mpu_index = 0;
      tool_menu_index = 0;
      displayTOOLMenu(tool_menu_index);
    }
  } 
  if (mpu_check) FusedAngle_Update(0.98f);
  Debug_MPU();
}
void Debug_MPU(void) {
  if(DelayMillis(100)){
    Serial.print(F("Pitch: "));
    Serial.print(mpu.pitch, 2);
    Serial.print(F(" | Roll: "));
    Serial.print(mpu.roll, 2);
    Serial.print(F(" | Yaw: "));
    Serial.println(mpu.yaw, 2);
  }
}

// ================== Getters ==================
float MPU_GetRoll(void)  { return mpu.roll; }
float MPU_GetPitch(void) { return mpu.pitch; }
float MPU_GetYaw(void)   { return mpu.yaw;  }

int16_t MPU_GetRawGX(void){ return mpu.gx; }
int16_t MPU_GetRawGY(void){ return mpu.gy; }
int16_t MPU_GetRawGZ(void){ return mpu.gz; }

int16_t MPU_GetRawAX(void){ return mpu.ax; }
int16_t MPU_GetRawAY(void){ return mpu.ay; }
int16_t MPU_GetRawAZ(void){ return mpu.az; }
