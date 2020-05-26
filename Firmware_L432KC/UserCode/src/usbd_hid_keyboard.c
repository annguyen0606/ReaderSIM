#include "usbd_hid_keyboard.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_hid.h"
#define PRESS_REPORT_SIZE 10
#define KB_DELAY 17

uint8_t press_report[PRESS_REPORT_SIZE];
uint8_t ASCII_to_Keyboard[255][2];


extern USBD_HandleTypeDef USBD_Device;

//extern PCD_HandleTypeDef hpcd_USB_FS;

void ASCII_to_Keyboard_init (void);

USBD_StatusTypeDef USBD_HID_KEYBOARD_Init (void)
{
    ASCII_to_Keyboard_init();
    /* Init Device Library */
    USBD_StatusTypeDef status = USBD_Init (&USBD_Device, &HID_Desc, 0);
    
    if (status == USBD_OK)
        {
            /* Register the HID class */
            status = USBD_RegisterClass (&USBD_Device, &USBD_HID);
            
            if (status == USBD_OK)
                {
                    /* Start Device Process */
                    status = USBD_Start (&USBD_Device);
                }
        }
        
    return status;
}

void ASCII_to_Keyboard_init()
{
    ASCII_to_Keyboard['0'][0] = KEY_0_CPARENTHESIS;
    ASCII_to_Keyboard['1'][0] = KEY_1_EXCLAMATION_MARK;
    ASCII_to_Keyboard['2'][0] = KEY_2_AT;
    ASCII_to_Keyboard['3'][0] = KEY_3_NUMBER_SIGN;
    ASCII_to_Keyboard['4'][0] = KEY_4_DOLLAR;
    ASCII_to_Keyboard['5'][0] = KEY_5_PERCENT;
    ASCII_to_Keyboard['6'][0] = KEY_6_CARET;
    ASCII_to_Keyboard['7'][0] = KEY_7_AMPERSAND;
    ASCII_to_Keyboard['8'][0] = KEY_8_ASTERISK;
    ASCII_to_Keyboard['9'][0] = KEY_9_OPARENTHESIS;
    ASCII_to_Keyboard['a'][0] = KEY_A;
    ASCII_to_Keyboard['b'][0] = KEY_B;
    ASCII_to_Keyboard['c'][0] = KEY_C;
    ASCII_to_Keyboard['d'][0] = KEY_D;
    ASCII_to_Keyboard['e'][0] = KEY_E;
    ASCII_to_Keyboard['f'][0] = KEY_F;
    ASCII_to_Keyboard['g'][0] = KEY_G;
    ASCII_to_Keyboard['h'][0] = KEY_H;
    ASCII_to_Keyboard['i'][0] = KEY_I;
    ASCII_to_Keyboard['j'][0] = KEY_J;
    ASCII_to_Keyboard['k'][0] = KEY_K;
    ASCII_to_Keyboard['l'][0] = KEY_L;
    ASCII_to_Keyboard['m'][0] = KEY_M;
    ASCII_to_Keyboard['n'][0] = KEY_N;
    ASCII_to_Keyboard['o'][0] = KEY_O;
    ASCII_to_Keyboard['p'][0] = KEY_P;
    ASCII_to_Keyboard['q'][0] = KEY_Q;
    ASCII_to_Keyboard['r'][0] = KEY_R;
    ASCII_to_Keyboard['s'][0] = KEY_S;
    ASCII_to_Keyboard['t'][0] = KEY_T;
    ASCII_to_Keyboard['u'][0] = KEY_U;
    ASCII_to_Keyboard['v'][0] = KEY_V;
    ASCII_to_Keyboard['w'][0] = KEY_W;
    ASCII_to_Keyboard['x'][0] = KEY_X;
    ASCII_to_Keyboard['y'][0] = KEY_Y;
    ASCII_to_Keyboard['z'][0] = KEY_Z;
    ASCII_to_Keyboard['A'][0] = KEY_A;
    ASCII_to_Keyboard['B'][0] = KEY_B;
    ASCII_to_Keyboard['C'][0] = KEY_C;
    ASCII_to_Keyboard['D'][0] = KEY_D;
    ASCII_to_Keyboard['E'][0] = KEY_E;
    ASCII_to_Keyboard['F'][0] = KEY_F;
    ASCII_to_Keyboard['G'][0] = KEY_G;
    ASCII_to_Keyboard['H'][0] = KEY_H;
    ASCII_to_Keyboard['I'][0] = KEY_I;
    ASCII_to_Keyboard['J'][0] = KEY_J;
    ASCII_to_Keyboard['K'][0] = KEY_K;
    ASCII_to_Keyboard['L'][0] = KEY_L;
    ASCII_to_Keyboard['M'][0] = KEY_M;
    ASCII_to_Keyboard['N'][0] = KEY_N;
    ASCII_to_Keyboard['O'][0] = KEY_O;
    ASCII_to_Keyboard['P'][0] = KEY_P;
    ASCII_to_Keyboard['Q'][0] = KEY_Q;
    ASCII_to_Keyboard['R'][0] = KEY_R;
    ASCII_to_Keyboard['S'][0] = KEY_S;
    ASCII_to_Keyboard['T'][0] = KEY_T;
    ASCII_to_Keyboard['U'][0] = KEY_U;
    ASCII_to_Keyboard['V'][0] = KEY_V;
    ASCII_to_Keyboard['W'][0] = KEY_W;
    ASCII_to_Keyboard['X'][0] = KEY_X;
    ASCII_to_Keyboard['Y'][0] = KEY_Y;
    ASCII_to_Keyboard['Z'][0] = KEY_Z;
    ASCII_to_Keyboard['A'][1] = KEY_LEFTSHIFT;
    ASCII_to_Keyboard['B'][1] = KEY_LEFTSHIFT;
    ASCII_to_Keyboard['C'][1] = KEY_LEFTSHIFT;
    ASCII_to_Keyboard['D'][1] = KEY_LEFTSHIFT;
    ASCII_to_Keyboard['E'][1] = KEY_LEFTSHIFT;
    ASCII_to_Keyboard['F'][1] = KEY_LEFTSHIFT;
    ASCII_to_Keyboard['G'][1] = KEY_LEFTSHIFT;
    ASCII_to_Keyboard['H'][1] = KEY_LEFTSHIFT;
    ASCII_to_Keyboard['I'][1] = KEY_LEFTSHIFT;
    ASCII_to_Keyboard['J'][1] = KEY_LEFTSHIFT;
    ASCII_to_Keyboard['K'][1] = KEY_LEFTSHIFT;
    ASCII_to_Keyboard['L'][1] = KEY_LEFTSHIFT;
    ASCII_to_Keyboard['M'][1] = KEY_LEFTSHIFT;
    ASCII_to_Keyboard['N'][1] = KEY_LEFTSHIFT;
    ASCII_to_Keyboard['O'][1] = KEY_LEFTSHIFT;
    ASCII_to_Keyboard['P'][1] = KEY_LEFTSHIFT;
    ASCII_to_Keyboard['Q'][1] = KEY_LEFTSHIFT;
    ASCII_to_Keyboard['R'][1] = KEY_LEFTSHIFT;
    ASCII_to_Keyboard['S'][1] = KEY_LEFTSHIFT;
    ASCII_to_Keyboard['T'][1] = KEY_LEFTSHIFT;
    ASCII_to_Keyboard['U'][1] = KEY_LEFTSHIFT;
    ASCII_to_Keyboard['V'][1] = KEY_LEFTSHIFT;
    ASCII_to_Keyboard['W'][1] = KEY_LEFTSHIFT;
    ASCII_to_Keyboard['X'][1] = KEY_LEFTSHIFT;
    ASCII_to_Keyboard['Y'][1] = KEY_LEFTSHIFT;
    ASCII_to_Keyboard['Z'][1] = KEY_LEFTSHIFT;
    ASCII_to_Keyboard[':'][0] = KEY_SEMICOLON_COLON;
    ASCII_to_Keyboard[':'][1] = KEY_LEFTSHIFT;
    ASCII_to_Keyboard['/'][0] = KEY_SLASH_QUESTION;
    ASCII_to_Keyboard['.'][0] = KEY_DOT_GREATER;
    ASCII_to_Keyboard['-'][0] = KEY_MINUS_UNDERSCORE;
    ASCII_to_Keyboard[' '][0] = KEY_SPACEBAR;
    ASCII_to_Keyboard['.'][0] = KEY_DOT_GREATER;
    ASCII_to_Keyboard[0x0D][0] = KEY_ENTER;
}

uint8_t USBD_HID_KEYBOARD_SendKey (uint8_t asciikey)
{
    if (ASCII_to_Keyboard[asciikey][1] != 0x00)
        press_report[0] = 0x01 << (ASCII_to_Keyboard[asciikey][1] & 0x0F);
        
    press_report[2] = (ASCII_to_Keyboard[asciikey][0]);
    uint8_t status = USBD_HID_SendReport (&USBD_Device, press_report, PRESS_REPORT_SIZE);
    HAL_Delay (KB_DELAY);
    press_report[0] = KEY_NONE;
    press_report[2] = KEY_NONE;
    status |= USBD_HID_SendReport (&USBD_Device, press_report, PRESS_REPORT_SIZE);
    HAL_Delay (KB_DELAY);
    return status;
}


