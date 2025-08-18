#ifndef OLED_H
#define OLED_H

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define I2C_ADDRESS   0x3C
#define OLED_RESET    -1

#include "Arduino.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#include "Wire.h"
#include "DELAY.h"




extern void Oled_Init(void);
extern void displayHomeScreen(void);
extern void displayMainMenu(uint8_t selected);
extern void displayPIDMenu(uint8_t selected);
extern void displayGainMenu(uint8_t selected);
extern void displayPitchRollMenu(uint8_t selected, const float values[]);
extern void displayYawMenu(uint8_t selected, const float values[]);
extern void displaySelectedGain(float gain) ;
extern void displayTOOLMenu(uint8_t selected);
extern void displayNoteReset(uint8_t selected);
extern void displayNRFMenu(uint8_t selected);
extern void displayMPUMenu(uint8_t selected);
extern void displayCaliLoading(uint8_t percent, bool cali_check);


#endif