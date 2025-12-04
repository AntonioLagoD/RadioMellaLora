const LoRaWANBand_t Region = EU868;
// OTAA  -- NON SE USAN
uint64_t joinEUI = 0x0000000000000000;
uint64_t devEUI = 0;
uint8_t appKey[] = {  };
uint8_t nwkKey[] = {  };

// CLAVES ABP 
uint32_t devAddr = 0xFFFFFFFF; 
// En LORAWAN 1.0.3 estas duas non se usan
//uint8_t fNwkSIntKey[] = {  };
//uint8_t sNwkSIntKey[] = {  };
uint8_t nwkSEncKey[] = {  }; // Aquí poñer NwkSKey formatada. Por exemplo : <0xF8>, <0x8E>, ....
uint8_t appSKey[] = {  };



