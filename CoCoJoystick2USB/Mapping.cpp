#include "Mapping.h"

int Mapping::map(int real) {
  int mapped;
  if(real >= input.centerMinimum && real <= input.centerMaximum) {
    mapped = outputCenter; 
  }
  if(real < input.centerMinimum) {
    mapped = ::map(real, input.edgeMinimum, input.centerMinimum, outputMinimum, outputCenter); 
  }
  if(real > input.centerMaximum) {
    mapped = ::map(real, input.centerMaximum, input.edgeMaximum, outputCenter, outputMaximum); 
  }
  return mapped;
}

void Mapping::print() {
  Serial.print("output = [ ");
  Serial.print(outputMinimum);
  Serial.print(", ");
  Serial.print(outputMaximum);
  Serial.print(", (");
  Serial.print(outputCenter);

  Serial.print(")] edge = [ ");
  Serial.print(input.edgeMinimum);
  Serial.print(", ");
  Serial.print(input.edgeMaximum);
  Serial.print(" ] center = [ ");
  Serial.print(input.centerMinimum);
  Serial.print(", ");
  Serial.print(input.centerMaximum);
  Serial.println(" ]");
}
