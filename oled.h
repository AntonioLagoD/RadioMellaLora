#ifndef OLED_H_
#define OLED_H_

#include <Wire.h> 
#include "SSD1306Wire.h"  

#define OLED_SDA    9      // GPIO17  -- OLED'S SDA
#define OLED_SCL    8      // GPIO18  -- OLED's SCL
#define OLED_RST    21      // GPIO21  -- OLED's RESET
#define OLED_ADDR   0x3c    // OLED's ADDRESS
#define Vext 21
void setupOLED(void);

extern SSD1306Wire display;

#endif