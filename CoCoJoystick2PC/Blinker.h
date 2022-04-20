/*
  Blinker.h - Library for reading joystick.
  Created by Gustavo Schoenaker
  Released into the public domain.
*/
#ifndef Blinker_h
#define Blinker_h

#define DEBUG

#include "config.h"
#include "Arduino.h"

#define null (void *)0

#define BLINK_MAX_TIMES 256

const uint16_t noBlinks[] = { 0 };
const uint16_t shortBliks[] = { 100, 100, 0 };
const uint16_t mediumBliks[] = { 200, 200, 0 };
const uint16_t longBliks[] = { 300, 300, 0 };
const uint16_t ditBlinks[] = { 200, 800, 0 };
const uint16_t doubleBliks[] = { 100, 100, 100, 600, 0 };
const uint16_t tripleBliks[] = { 100, 100, 100, 100, 100, 600, 0 };
const uint16_t onceBliks[] = { 200, 1000, 0 };
const uint16_t twiceBliks[] = { 200, 200, 200, 1000, 0 };

class Blinker {
  public:
    Blinker() {};
    void setup(uint8_t pin, bool highIsOn = true) { _pin = pin; _on = highIsOn ? HIGH : LOW; _off = highIsOn ? LOW : HIGH; playOff(); setNoFade(); };
    void loop(uint32_t now = millis());
    void play(const uint16_t *timeArray, int repeat = -1);
    void playOff() { _repeat = 0; off(); };
    void playOn() { _repeat = 0; on(); };
    void setFade(int intensityStep = 5, int timeStep = 30) { _intensityStep = intensityStep; _timeStep = timeStep; };
    void setNoFade() { setFade(255, 0); }
    
  protected:
    int _intensityStep;
    int _timeStep;
    int _value;
    bool _turningOn;
    void _fade(int16_t d) { _value += d; if(_value<0) _value = 0; if(_value>255) _value = 255; analogWrite(_pin, _value); debugvar("fade = ", _value); debugvar(" d = ", d); debugln(""); };
    uint32_t _nextFade = 0;
    
    void on() { _turningOn = true; };
    void off() { _turningOn = false; };
    uint8_t _pin;
    uint8_t _on;
    uint8_t _off;
    uint32_t _start = 0;
    int _repeat = 0;
    uint16_t _position;
    uint16_t *_playTimes;
    uint32_t _playUntil = 0;
    void _play();
    void _reset() {
      _position = 0;
      _start = millis();
      on();
    }
};

#endif
