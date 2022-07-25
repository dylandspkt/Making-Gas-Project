/*
 * ks0108_Mega.h - User specific configuration for openGLCD library
 *
 * Use this file to set io pins
 * when using an Arduino Mega1280 or Mega2560 board
 *
 * The configuration below uses a single port for data 
 *
 */

#ifndef GLCD_PIN_CONFIG_H
#define GLCD_PIN_CONFIG_H

/*
 * define name for pin configuration
 */
#define glcd_PinConfigName "ks0108-Mega"

/*********************************************************/
/*  Configuration for assigning LCD bits to Arduino Pins */
/*********************************************************/

/*
 * Pins can be assigned using Arduino pin numbers 0-n
 * Pins can also be assigned using PIN_Pb 
 *    where P is port A-L and b is bit 0-7
 *    Example: port D bit 3 is PIN_D3
 *
 */

/*
 * Data pin definitions
 * This version uses pins 22-29 for LCD Data 
 */
// #define glcdPinData0    22
// #define glcdPinData1    23
// #define glcdPinData2    24
// #define glcdPinData3    25
// #define glcdPinData4    26
// #define glcdPinData5    27
// #define glcdPinData6    28
// #define glcdPinData7    29
#define glcdPinData0    15
#define glcdPinData1    13
#define glcdPinData2    11
#define glcdPinData3    9
#define glcdPinData4    7
#define glcdPinData5    5
#define glcdPinData6    3
#define glcdPinData7    2




/*
 * Pins used for Control
 */
#define glcdPinCSEL1     4    // CS1 Bit  
#define glcdPinCSEL2     6    // CS2 Bit
#define glcdPinRW        23    // R/W Bit
#define glcdPinDI        25    // D/I Bit 
#define glcdPinEN        17    // EN Bit

#if NBR_CHIP_SELECT_PINS > 2
#define glcdPinCSEL3     32   // third chip select if needed
#endif

#if NBR_CHIP_SELECT_PINS > 3
#define glcdPinCSEL4     31   // fourth chip select if needed
#endif

// Reset - uncomment the next line if glcd module reset is connected to an Arduino pin
//#define glcdPinRES         30    // optional s/w Reset control

//#define glcdPinBL	XX // optional backlight control pin controls BL circuit

#endif //GLCD_PIN_CONFIG_H
