#ifndef BUTTON_H
#define BUTTON_H

//#include "OneButton.h"
#include "Arduino.h"
#include "MPU6050.h"
#include "ESP32BUTTON.h"

#define BUTTON_UP     16
#define BUTTON_ENTER  17
#define BUTTON_DOWN   13

extern bool reset_check, mpu_check, nrf_check, cali_check;
extern int main_menu_index, pid_menu_index, tool_menu_index,  gain_menu_index, reset_menu_index;
extern int pitch_roll_index, yaw_index, mpu_index, nrf_index;

typedef enum {
    HOME = 0,
    MAIN_MENU,
    PID_MENU,
    GAIN_MENU,
    PID_PITCH_ROLL,
    PID_YAW,
    TOOL_MENU,
    NRF_MENU,
    MPU_MENU, 
    RESET_MENU  
} MenuState;


extern MenuState currentMenu;

extern void Button_Init(void);
extern void Button_Loop();
extern void Debug(void);

#endif