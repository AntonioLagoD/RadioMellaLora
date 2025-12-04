/*******************************************************************************
  # PAYLOAD FORMATER para TTN #
  function Decoder(bytes, port) {
    var decoded = {};
    decoded.latitude = ((bytes[0]<<16)>>>0) + ((bytes[1]<<8)>>>0) + bytes[2];
    decoded.latitude = (decoded.latitude / 16777215.0 * 180) - 90;
    decoded.longitude = ((bytes[3]<<16)>>>0) + ((bytes[4]<<8)>>>0) + bytes[5];
    decoded.longitude = (decoded.longitude / 16777215.0 * 360) - 180;
    var altValue = ((bytes[6]<<8)>>>0) + bytes[7];
    var sign = bytes[6] & (1 << 7);
    if(sign)
    {
        decoded.altitude = 0xFFFF0000 | altValue;
    }
    else
    {
        decoded.altitude = altValue;
    }
    decoded.hdop = bytes[8] / 10.0;
    return decoded;
  }
*******************************************************************************/
#include <Arduino.h>
#include <RadioLib.h>
#include "gps.h"
#include "oled.h"
#include "ttn.h"
#include "pins.h"

#define RETARDO 10000

// Crear instancia SPI personalizada usando HSPI
//SPIClass customSPI(HSPI);
SPIClass customSPI= SPIClass(HSPI);
// Crear instancia de radio con SPI personalizado
SX1276 radio = new Module(CUSTOM_CS, DIO0_PIN, RST_PIN, DIO1_PIN, customSPI);

LoRaWANNode node(&radio, &Region);
uint8_t txBuffer[9];

bool loraOk = false;

void setup()
{
  bool otaa = false;
  int16_t estadoLoRa = -1;
  Serial.begin(9600);
  delay(200); // Damos tiempo a abrir la terminal serie
  Serial.println("Setup OLED");
  setupOLED();
  Serial.println("Iniciando TTN Mapper...");
  Serial.println("Iniciando LoRa con SPI personalizado...");
  
  // Inicializar SPI personalizado con los pines definidos
  customSPI.begin(CUSTOM_SCK, CUSTOM_MISO, CUSTOM_MOSI, CUSTOM_CS);

  // Configuración chip LoRa
  estadoLoRa = radio.begin();
  if ( estadoLoRa == RADIOLIB_ERR_NONE) {
    Serial.println("Chip LoRa configurado OK");
  }
  else
  {
    Serial.print("ERROR configurando chip LoRa! : "); 
	Serial.println(estadoLoRa);
    while(1) {delay(1);}
  }

  // Provisionado de claves OTAA
  if (otaa) {
	// LORAWAN 1.1 necesita nwkKey:
	//estadoLoRa = node.beginOTAA(joinEUI, devEUI, nwkKey, appKey);
    // LORAWAN 1.03 no necesita nwkKey. Se pasa NULL
	estadoLoRa = node.beginOTAA(joinEUI, devEUI, NULL, appKey);	
    if (estadoLoRa == RADIOLIB_ERR_NONE ) {
      Serial.println("Provisionado de claves OTAA OK");
	}
    else  {
      Serial.print("ERROR provisionando claves OTAA! : "); 
	  Serial.println(estadoLoRa);
      while(1) {delay(1);}
    }

    // Activación OTAA
    estadoLoRa = node.activateOTAA();
	Serial.println("Activación OTAA OK");
  }
  // Provisionado de claves ABP
  else {
	estadoLoRa = node.beginABP(devAddr, NULL, NULL, nwkSEncKey, appSKey);
	//Para LORAWAN 1.1.0
	//estadoLoRa = node.beginABP(devAddr, fNwkSIntKey, sNwkSIntKey, nwkSEncKey, appSKey);
    if ( estadoLoRa == RADIOLIB_ERR_NONE ) {
      Serial.println("Provisionado de claves ABP OK");
	  }
    else
    {
      Serial.print("ERROR provisionando claves ABP!:");
	    Serial.println(estadoLoRa);
      while(1) {delay(1);}
    }
    // Activación ABP	
    estadoLoRa = node.activateABP();
	  Serial.println("Activación ABP OK");
  }
  Serial.println("Setup GPS");
  setupGPS();
  
}
void loop()
{
  loraOk = false;
  if (checkGPSFix()) // Comprueba que tenemos GPS fix
  {
	buildGPSpacket(txBuffer); // rellenado payload
    // Envío paquete LoRaWAN
    Serial.print("Enviando paquete LoRaWAN...");
    int state = node.sendReceive(txBuffer, sizeof(txBuffer));    
    // Gestión de errores
    if (state < RADIOLIB_ERR_NONE) {
      Serial.print("Error en sendReceive : ");
      Serial.println(state);
    }
    else {
      Serial.println("Paquete enviado.");
      loraOk = true;
	    if(state > 0) Serial.println("Recibido downlink\n");      
	  }

  }
  else {
    Serial.println("No GPS Fix.");
	  char t[32];
    sprintf(t, "location valid: %i" , gps.location.isValid());
    Serial.println(t);
    sprintf(t, "location age: %i" , gps.location.age());
    Serial.println(t);
    sprintf(t, "hdop valid: %i" , gps.hdop.isValid());
    Serial.println(t);
    sprintf(t, "hdop age: %i" , gps.hdop.age());
    Serial.println(t);
    sprintf(t, "hdop: %i" , gps.hdop.value());
    Serial.println(t);
    sprintf(t, "altitude valid: %i" , gps.altitude.isValid());
    Serial.println(t);
    sprintf(t, "altitude age: %i" , gps.altitude.age());
    Serial.println(t);
    Serial.println();
  }
  printGPSdata(); // Mostramos datos GPS en pantalla OLED
  smartGPSdelay(RETARDO);
}
