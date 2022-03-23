#include "util.h"

void printHex(uint8_t *data, uint32_t size, bool println = false) {
  for(uint32_t i = 0; i < size; i++) {
    if(i > 0) Serial.print(" ");
    if(data[i] < 16) Serial.print("0");
    Serial.print(data[i], HEX);
  }
  if(println) Serial.println("");
}
