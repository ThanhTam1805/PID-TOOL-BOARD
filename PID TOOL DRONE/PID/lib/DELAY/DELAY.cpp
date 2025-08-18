#include "DELAY.h"
// Hàm tạo delay không chặn chương trình
bool DelayMillis(uint16_t time_ms) {
    static unsigned long start_time = 0;
    static bool running = false;

    if (!running) {
        start_time = millis();
        running = true;
    }

    if (millis() - start_time >= time_ms) {
        running = false; 
        return true;     
    }

    return false; 
}
