#include "OLED.h"


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


void Oled_Init(void){
    Wire.begin(); 
    bool status = display.begin(SSD1306_SWITCHCAPVCC, I2C_ADDRESS);
    if (!status) {
        Serial.println("OLED init failed!");
    }
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.display();
}

const char* paramNames[] = {"KP", "KI", "KD"};

void displayHomeScreen(void) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("Welcome to PID TOOL"));
  display.println(F("-------------------"));
  display.println(F("Press ENTER to go to"));
  display.println(F("the MAIN MENU"));
  display.display();
}

void displayMainMenu(uint8_t selected) {
  const char* items[] = {"PID MENU", "TOOL MENU"};
  
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(F("MAIN MENU"));
  display.println(F("-------------------"));
  
  for(uint8_t i = 0; i < 2; i++) {
    if(i == selected) display.print(F(">  "));
    else display.print(F("  "));
    display.println(items[i]);
  }
  
  display.display();
}

void displayPIDMenu(uint8_t selected) {
  const char* items[] = {"GAIN MENU", "PID PITCH AND ROLL", "PID YAW"};
  
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(F("PID MENU"));
  display.println(F("-------------------"));
  
  for(uint8_t i = 0; i < 3; i++) {
    if(i == selected) display.print(F(">  "));
    else display.print(F("  "));
    display.println(items[i]);
  }
  
  display.display();
}

void displayGainMenu(uint8_t selected) {
  const char* items[] = {"0.001", "0.01", "0.1", "1.0"};

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(F("GAIN MENU"));
  display.println(F("-------------------"));

  
  for(uint8_t i = 0; i < 4; i++) {
    if(i == selected) display.print(F(">  "));
    else display.print(F("  "));
    display.println(items[i]);
  }
  
  display.display();
}

void displayPitchRollMenu(uint8_t selected, const float values[]) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(F("PID PITCH & ROLL"));
  display.println(F("-------------------"));
  
  for(uint8_t i = 0; i < 3; i++) {
    if(i == selected) display.print(F(">  "));
    else display.print(F("  "));
    display.print(paramNames[i]);
    display.print(": ");
    display.println(values[i], 4);
  }
  
  display.println(F("Long press enter"));
  display.println(F("to Save & Exit"));
  display.display();
}

void displayYawMenu(uint8_t selected, const float values[]) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(F("PID YAW"));
  display.println(F("-------------------"));
  
  for(uint8_t i = 0; i < 3; i++) {
    if(i == selected) display.print(F(">  "));
    else display.print(F("  "));
    display.print(paramNames[i]);
    display.print(": ");
    display.println(values[i], 4);
  }
  display.println(F("Long press enter"));
  display.println(F("to Save & Exit"));
  display.display();
}

void displaySelectedGain(float gain) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print(F("SELECTED GAIN: "));
  display.println(gain, 3);
  display.display();
}

void displayTOOLMenu(uint8_t selected){
  const char* items[] = {"NRF", "HEAD TRACK", "RESET"};

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(F("TOOL"));
  display.println(F("-------------------"));

  for(uint8_t i = 0; i < 3; i++){
    if(i == selected) display.print(F(">  "));
    else display.print(F("  "));
    display.println(items[i]);
  }
  display.display();  
}
void displayNoteReset(uint8_t selected){
  const char* items[] = {"YES", "NO"};
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(F("RESET"));
  display.println(F("-------------------"));
  display.println(F("Are you sure you"));
  display.println(F("reset the module?"));
  display.println(F(" "));
  for(uint8_t i = 0; i < 2; i++){
    if(i == selected) display.print(F(">  "));
    else display.print(F("  "));
    display.println(items[i]);
  }
  display.display();
  
}

void displayNRFMenu(uint8_t selected){
  const char* items[] = {"ENABLE", "DISABLE"};
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(F("NRF MODULE"));
  display.println(F("-------------------"));
  display.println(F("Are you sure start"));
  display.println(F("the module ?"));

  for(uint8_t i = 0; i < 2; i++){
    if(i == selected) display.print(F(">  "));
    else display.print(F("  "));
    display.println(items[i]);
  }
  display.display();

}

void displayMPUMenu(uint8_t selected){
  const char* items[] = {"ENABLE", "DISABLE"};
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(F("Head Track"));
  display.println(F("-------------------"));
  display.println(F("Are you sure start"));
  display.println(F("the module ?"));

  for(uint8_t i = 0; i < 2; i++){
    if(i == selected) display.print(F(">  "));
    else display.print(F("  "));
    display.println(items[i]);
  }
  display.display();
}

void displayCaliLoading(uint8_t percent, bool cali_check) {
  static uint8_t dots = 0;

  display.clearDisplay();
  display.setCursor(0, 0);
  display.print(F("Calibration: "));
  display.print(percent);
  display.println(F("%"));
  display.println(F("-------------------"));
  display.setCursor(0, 30);
  if (!DelayMillis(100) && cali_check){
      display.print(F("Keep device still "));
      for (uint8_t i = 0; i < dots; i++) display.print('.');
      dots = (dots + 1) % 4;
  }
  display.display();
  
}
