/**
 * Mapping.h - Maps a real (ex: 10 bit unsiigned) to a logical value (16 bits signed), preserving the center values
 * Created by Gustavo Schoenaker on 2022
 *     
 * #DEFINES:
 *
 */
#ifndef Mapping_h
#define Mapping_h

#include "config.h"
#include "Arduino.h"

typedef struct {
    int edgeMinimum;
    int edgeMaximum;
    int centerMinimum;
    int centerMaximum;
} mappingData_t;

class Mapping
{
  public:
    mappingData_t input;

    int outputMinimum;
    int outputMaximum;
    int outputCenter;

    void setInput(mappingData_t newValue) { memmove(&input, &newValue, sizeof(input)); };
    mappingData_t *getInput() { return &input; };
    int centerValue() { return (input.centerMinimum + input.centerMaximum) / 2; };
    
  	Mapping() {};
    
    int map(int real);
    
    void print();
};

#endif
