#include <HardwareSerial.h>
#include "TinyGPS++.h" //https://github.com/mikalhart/TinyGPSPlus
#include "gps.h"
#include "oled.h"

TinyGPSPlus gps; // Objeto TinyGPS++
HardwareSerial gpsSerial(1); // Conexión Serie con GPS

extern bool loraOk;

// Versión custom de delay() para no perder comunicaciones con el GPS
void smartGPSdelay(unsigned long ms)
{
  unsigned long start = millis();
  do
  {
    while (gpsSerial.available())
      gps.encode(gpsSerial.read());
  } while (millis() - start < ms);
}

void setupGPS(void)
{
  gpsSerial.begin(GPS_BAUDRATE, SERIAL_8N1, GPS_TX_PIN, GPS_RX_PIN);
}

void printGPSdata(void)
{
  char text[32];
  display.clear();
  display.setFont(ArialMT_Plain_10);
  sprintf(text, "%02d/%02d/%02d ", gps.date.day(), gps.date.month(), gps.date.year());
  display.drawString(64,0, text);
  Serial.println(text);
  sprintf(text, "LoRa : %s", (loraOk) ? "OK" : "NO");
  display.drawString(0,10, text);
  sprintf(text, "%02d:%02d:%02d UTC", gps.time.hour(), gps.time.minute(), gps.time.second());
  display.drawString(64,10, text);
  Serial.println(text);
  
  if(gps.location.isValid())  {
    sprintf(text, "GPS : OK");
	display.drawString(0, 0, text);
	sprintf(text, "Lat:    %.6f", gps.location.lat());
    display.drawString(0,22, text);
    Serial.println(text);
    sprintf(text, "Long:  %.6f", gps.location.lng());
    display.drawString(0,32, text);
    Serial.println(text);
  }
  else {
    sprintf(text, "GPS : NO");
    display.drawString(0, 0, text);  
  }

  if(gps.altitude.isValid())
  {
    sprintf(text, "Alt:     %.1fm", gps.altitude.meters());
    display.drawString(0,42, text);
    Serial.println(text);
  }

  if(gps.speed.isValid())
  {
    sprintf(text, "Speed: %.0f Km/h", gps.speed.kmph());
    display.drawString(0,52, text);
    Serial.println(text);
  }

  sprintf(text, "Sats:  %i", gps.satellites.value());
  display.drawString(82,42, text);
  Serial.println(text);

  sprintf(text, "Hdop:%.1f", gps.hdop.hdop());
  display.drawString(82,52, text); 
  Serial.println(text);

  display.display();
}

void buildGPSpacket(uint8_t txBuffer[9])
{
  uint32_t LatitudeBinary, LongitudeBinary;
  uint16_t altitudeGps;
  uint8_t hdopGps;
  LatitudeBinary = ((gps.location.lat() + 90) / 180.0) * 16777215;
  LongitudeBinary = ((gps.location.lng() + 180) / 360.0) * 16777215;
  
  txBuffer[0] = ( LatitudeBinary >> 16 ) & 0xFF;
  txBuffer[1] = ( LatitudeBinary >> 8 ) & 0xFF;
  txBuffer[2] = LatitudeBinary & 0xFF;

  txBuffer[3] = ( LongitudeBinary >> 16 ) & 0xFF;
  txBuffer[4] = ( LongitudeBinary >> 8 ) & 0xFF;
  txBuffer[5] = LongitudeBinary & 0xFF;

  altitudeGps = gps.altitude.meters();
  txBuffer[6] = ( altitudeGps >> 8 ) & 0xFF;
  txBuffer[7] = altitudeGps & 0xFF;

  hdopGps = gps.hdop.hdop()/10;
  txBuffer[8] = hdopGps & 0xFF;
}

bool checkGPSFix(void)
{
  if (gps.location.isValid() && 
      gps.location.age() < 2000 &&
      gps.hdop.isValid() &&
      gps.hdop.hdop() <= 300 &&
      gps.hdop.age() < 2000 &&
      gps.altitude.isValid() && 
      gps.altitude.age() < 2000 ) return true;
  
  else return false;
}
