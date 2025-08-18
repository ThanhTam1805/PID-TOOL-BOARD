#include "BUTTON.h"
#include "OLED.h"

float gians[] = {0.001, 0.01, 0.1, 1.0};
float GIANS = 0.001;

int main_menu_index = 0, pid_menu_index = 0, tool_menu_index = 0,  gain_menu_index = 0, reset_menu_index = 0;
int pitch_roll_index = 0, yaw_index = 0, mpu_index = 0, nrf_index = 0;

/// @brief //////////////////////////////////
int pitchRollParamIndex = 0;
bool editingPitchRoll = false;
bool selectedPitchRoll = false;

int yawParamIndex = 0;
bool editingYaw = false;

float pid_pitch_roll[3] = {0.0, 0.0, 0.0};  // Kp, Ki, Kd
float pid_yaw[3] = {0.0, 0.0, 0.0};        // Kp, Ki, Kd


////////////////////////////////////////////
bool reset_check = false, nrf_check = false, mpu_check = false, cali_check = false;
MenuState currentMenu;
// Khởi tạo OneButton object
ESP32Button BTs[] = {
    ESP32Button(BUTTON_UP,    true),
    ESP32Button(BUTTON_ENTER, true),
    ESP32Button(BUTTON_DOWN,  true)
};

// Enum để xác định nút nào
enum ButtonIndex {
    BTN_UP = 0,
    BTN_ENTER,
    BTN_DOWN
};

// Menu con của menu main
enum {
    PID = 0,
    TOOL
};
// Menu con của menu tool
enum {
    NRF = 0,
    MPU,
    RESET
};
// Menu con của menu pid
enum {
    GAIN = 0,
    PITCH_ROLL,
    YAW
};
// Menu con của menu Pitch & Roll, Yaw
enum {
    KP = 0,
    KI,
    KD
};

enum{
    YES = 0,
    NO
};
enum{
    ENABLE,
    DISABLE,
    CALIRATE
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
===================================================================Button handle=========================================================
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void HandleClick(ButtonIndex idx) {
    switch (idx) {
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
===================================================================Button UP============================================================
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        case BTN_UP:
            switch (currentMenu) {
                case HOME:
                    break;
                case MAIN_MENU:
                    main_menu_index = constrain(main_menu_index - 1, 0, 1);
                    displayMainMenu(main_menu_index);
                    break;

                case PID_MENU:
                    pid_menu_index = constrain(pid_menu_index - 1, 0, 2);
                    displayPIDMenu(pid_menu_index);
                    break;
                case TOOL_MENU:
                    tool_menu_index = constrain(tool_menu_index - 1, 0, 2);
                    displayTOOLMenu(tool_menu_index);
                    break;

                case GAIN_MENU:
                    gain_menu_index = constrain(gain_menu_index - 1, 0, 3);
                    displayGainMenu(gain_menu_index);
                    break;

                case PID_PITCH_ROLL:
                    if (editingPitchRoll) {
                        // Tăng giá trị PID hiện tại
                        pid_pitch_roll[pitch_roll_index] += GIANS;
                        pid_pitch_roll[pitch_roll_index] = constrain(pid_pitch_roll[pitch_roll_index], 0.0, 5.0);
                    } else {
                        // Di chuyển vị trí chọn (>)
                        pitch_roll_index = constrain(pitch_roll_index - 1, 0, 2); // 0:KP, 1:KI, 2:KD
                    }
                    displayPitchRollMenu(pitch_roll_index, pid_pitch_roll);
                    break;
                case PID_YAW:
                    if (editingYaw) {
                        // Giảm giá trị PID hiện tại
                        pid_yaw[yaw_index] += GIANS;
                        pid_yaw[yaw_index] = constrain(pid_yaw[yaw_index], 0.0, 5.0);
                    } else {
                        // Di chuyển vị trí chọn (>)
                        yaw_index = constrain(yaw_index - 1, 0, 2); // 0:KP, 1:KI, 2:KD
                    }
                    displayYawMenu(yaw_index, pid_yaw);
                    break;
                case RESET_MENU:
                    reset_menu_index = constrain(reset_menu_index - 1, 0, 1);
                    displayNoteReset(reset_menu_index);
                    break;
                case NRF_MENU:
                    nrf_index = constrain(nrf_index - 1, 0, 1);
                    displayNRFMenu(nrf_index);
                    break;
                case MPU_MENU:
                    mpu_index = constrain(mpu_index - 1, 0, 1);
                    displayMPUMenu(mpu_index);
                    break;
            }
            //Serial.println("UP Click");
            break;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
===================================================================Button DOWN============================================================
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        case BTN_DOWN:
            switch (currentMenu) {
                case HOME:
                    break;
                case MAIN_MENU:
                    main_menu_index = constrain(main_menu_index + 1, 0, 1);
                    displayMainMenu(main_menu_index);
                    break;

                case PID_MENU:
                    pid_menu_index = constrain(pid_menu_index + 1, 0, 2);
                    displayPIDMenu(pid_menu_index);
                    break;
                case TOOL_MENU:
                    tool_menu_index = constrain(tool_menu_index + 1, 0, 2);
                    displayTOOLMenu(tool_menu_index);
                    break;
                case GAIN_MENU:
                    gain_menu_index = constrain(gain_menu_index + 1, 0, 3);
                    displayGainMenu(gain_menu_index);
                    break;

                case PID_PITCH_ROLL:
                    if (editingPitchRoll) {
                        // Giảm giá trị PID hiện tại
                        pid_pitch_roll[pitch_roll_index] -= GIANS;
                        pid_pitch_roll[pitch_roll_index] = constrain(pid_pitch_roll[pitch_roll_index], 0.0, 5.0);
                    } else {
                        // Di chuyển vị trí chọn (>)
                        pitch_roll_index = constrain(pitch_roll_index + 1, 0, 2); // 0:KP, 1:KI, 2:KD
                    }
                    displayPitchRollMenu(pitch_roll_index, pid_pitch_roll);
                    break;
                case PID_YAW:
                    if (editingYaw) {
                        // Giảm giá trị PID hiện tại
                        pid_yaw[yaw_index] -= GIANS;
                        pid_yaw[yaw_index] = constrain(pid_yaw[yaw_index], 0.0, 5.0);
                    } else {
                        // Di chuyển vị trí chọn (>)
                        yaw_index = constrain(yaw_index + 1, 0, 2); // 0:KP, 1:KI, 2:KD
                    }
                    displayYawMenu(yaw_index, pid_yaw);
                    break;
                case RESET_MENU:
                    reset_menu_index = constrain(reset_menu_index + 1, 0, 1);
                    displayNoteReset(reset_menu_index);
                    break;

                case NRF_MENU:
                    nrf_index = constrain(nrf_index + 1, 0, 1);
                    displayNRFMenu(nrf_index);
                    break;

                case MPU_MENU:
                    mpu_index = constrain(mpu_index + 1, 0, 1);
                    displayMPUMenu(mpu_index);
                    break;

            }
            //Serial.println("DOWN Click");
            break;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
===================================================================Button ENTER=========================================================
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        case BTN_ENTER:
            //Serial.println("ENTER Click");
            switch (currentMenu) {
                case HOME:
                    currentMenu = MAIN_MENU;
                    displayMainMenu(main_menu_index);
                    break;

                case MAIN_MENU:
                    if (main_menu_index == PID) {
                        currentMenu = PID_MENU;
                        displayPIDMenu(pid_menu_index);
                    }
                    else if (main_menu_index == TOOL){
                        currentMenu = TOOL_MENU;
                        displayTOOLMenu(tool_menu_index);
                    }
                    break;

                case PID_MENU:
                    if (pid_menu_index == GAIN) {
                        currentMenu = GAIN_MENU;
                        displayGainMenu(gain_menu_index);
                    } else if (pid_menu_index == PITCH_ROLL) {
                        currentMenu = PID_PITCH_ROLL;
                        pitch_roll_index = 0;
                        editingPitchRoll = true; // Bắt đầu cho phép chỉnh KP ngay
                        displayPitchRollMenu(pitch_roll_index, pid_pitch_roll);
                    } else if (pid_menu_index == YAW){
                        currentMenu = PID_YAW;
                        yaw_index = 0;
                        editingYaw = true; // Bắt đầu cho phép chỉnh KP ngay
                        displayYawMenu(yaw_index, pid_yaw);
                    }
                    break;
                case TOOL_MENU:
                    if (tool_menu_index == RESET){
                        currentMenu = RESET_MENU;
                        displayNoteReset(reset_menu_index);
                    }
                    else if (tool_menu_index == NRF){
                        currentMenu = NRF_MENU;
                        displayNRFMenu(nrf_index);
                    }
                    else if (tool_menu_index == MPU){
                        currentMenu = MPU_MENU;
                        displayMPUMenu(mpu_index);
                    }
                    break;
                case NRF_MENU:
                    if (nrf_index == ENABLE){
                       // Tạo biến check trạng thái module
                       nrf_check = true;
                       // Về HOME 
                       main_menu_index = 0;
                       currentMenu = HOME;
                       displayHomeScreen();
                    }
                    else if (nrf_index == DISABLE){
                        // Tạo biến check trạng thái module
                        nrf_index = false;
                        // Lùi 1 cấp
                        currentMenu = TOOL_MENU;
                        tool_menu_index = 0;
                        displayTOOLMenu(tool_menu_index);
                    }
                    break;
                case MPU_MENU:
                    if (mpu_index == ENABLE){
                        //Tạo biến check trạng thái module
                        cali_check = true;
                    }
                    else if (mpu_index == DISABLE) {
                        // Lùi 1 cấp menu
                        mpu_check = false;
                        tool_menu_index = 0;
                        mpu_index = 0;
                        currentMenu = TOOL_MENU;
                        displayTOOLMenu(tool_menu_index);
                    }
                    break;


                case RESET_MENU:
                    if (reset_menu_index == YES){
                        // Tạo biến check để reset tất cả giá trị và thông tin của BOARD
                        reset_check = true;
                        // Reset tất cả biến quản lý menu
                        pid_menu_index = 0;
                        tool_menu_index = 0;
                        gain_menu_index = 0; 
                        reset_menu_index = 0;
                        main_menu_index = 0;
                        pitch_roll_index = 0; 
                        yaw_index = 0;
                        currentMenu = HOME;
                        displayHomeScreen();
                        
                    }
                    // Trở về menu tool (lùi 1 cấp)
                    else if (reset_menu_index == NO){
                        tool_menu_index = 0;
                        currentMenu = TOOL_MENU;
                        displayTOOLMenu(tool_menu_index);
                    }
                   break;
        
                case GAIN_MENU:
                    GIANS = gians[gain_menu_index];
                    displaySelectedGain(GIANS);
                    delay(1000);
                    currentMenu = PID_MENU;
                    gain_menu_index = 0;
                    displayPIDMenu(pid_menu_index);
                    break;

                case PID_PITCH_ROLL:
                    // Khi nhấn ENTER: bật/tắt chế độ chỉnh giá trị
                    editingPitchRoll = !editingPitchRoll;
                    displayPitchRollMenu(pitch_roll_index, pid_pitch_roll);
                    break;
                case PID_YAW:
                    // Khi nhấn ENTER: bật/tắt chế độ chỉnh giá trị
                    editingYaw = !editingYaw;
                    displayYawMenu(yaw_index, pid_yaw);
                    break;
                
            }
            break;
    }
}



void HandleLongPress(ButtonIndex idx) {
    switch (idx) {
        // case BTN_UP:
        //     Serial.println("UP Long Press");
        //     break;

        case BTN_ENTER:
            Serial.println("ENTER Long Press");
            switch (currentMenu) {
                case HOME:
                    break;
                case MAIN_MENU:
                    currentMenu = HOME;
                    displayHomeScreen();
                    break;
                case PID_MENU:
                    currentMenu = MAIN_MENU;
                    main_menu_index = 0;
                    displayMainMenu(main_menu_index);
                    break;
                case TOOL_MENU:
                    currentMenu = MAIN_MENU;
                    main_menu_index = 0;
                    displayMainMenu(main_menu_index);
                case NRF_MENU:
                case MPU_MENU:
                case RESET_MENU:
                case GAIN_MENU: // Không cần làm gì vì khi chỉnh gains xong thì tự động quay về PID MENU
                case PID_PITCH_ROLL:
                    currentMenu = PID_MENU;
                    pid_menu_index = 0;
                    displayPIDMenu(pid_menu_index);
                case PID_YAW:
                    currentMenu = PID_MENU;
                    pid_menu_index = 0;
                    displayPIDMenu(pid_menu_index);
                    break;
            }
            break;

        // case BTN_DOWN:
        //     Serial.println("DOWN Long Press");
        //     break;
    }
}



// Adapter để truyền index cho OneButton callback
void Click0() { HandleClick(BTN_UP); }
void Click1() { HandleClick(BTN_ENTER); }
void Click2() { HandleClick(BTN_DOWN); }

//void Long0() { HandleLongPress(BTN_UP); }
void Long1() { HandleLongPress(BTN_ENTER); }
//void Long2() { HandleLongPress(BTN_DOWN); }

void Button_Init() {
    BTs[BTN_UP].attachClick(Click0);
    BTs[BTN_ENTER].attachClick(Click1);
    BTs[BTN_DOWN].attachClick(Click2);

    //BTs[BTN_UP].attachLongPressStart(Long0);
    BTs[BTN_ENTER].attachLongPressStart(Long1);
    //BTs[BTN_DOWN].attachLongPressStart(Long2);
}

// Gọi mỗi vòng lặp
void Button_Loop() {
    for (int i = 0; i < 3; i++) {
        BTs[i].tick();
    }
}
void Debug(void){
    Serial.print("Main menu index: ");
    Serial.print(main_menu_index);
    Serial.print(" | PID menu index: ");
    Serial.print(pid_menu_index);
    Serial.print("Gain menu index: ");
    Serial.print(gain_menu_index);
    Serial.print(" | PID Pitch Roll index: ");
    Serial.print(pitch_roll_index);
    Serial.print(" | Gain: ");
    Serial.print(GIANS);
    Serial.println();
}