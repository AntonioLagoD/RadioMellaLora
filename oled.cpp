#include <Arduino.h>
#include <Wire.h> 
#include "SSD1306wire.h"  
#include "oled.h"

SSD1306Wire display(OLED_ADDR, OLED_SDA, OLED_SCL); // Objeto pantalla OLED

void setupOLED(void)
{
  Wire.begin(OLED_SDA, OLED_SCL);         
  delay(100); 
  // Activamos Vext (alimentación externa conectada a OLED)
  pinMode(Vext,OUTPUT);
  digitalWrite(Vext, LOW);

  pinMode(OLED_RST,OUTPUT);
  digitalWrite(OLED_RST, LOW);  // Ponemos a 0 para resetear OLED
  delay(50);
  digitalWrite(OLED_RST, HIGH); // Liberamos el reset poniéndolo a 1
  delay(50);

  display.init();
  display.flipScreenVertically();
  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 0, "TTN Mapper");
  display.display();
}
