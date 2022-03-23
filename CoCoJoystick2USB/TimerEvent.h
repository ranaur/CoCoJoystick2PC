/**
 * TimerEvent.h - Time handling routines  
 * 
 * Created by Gustavo Schoenaker
 */
#ifndef TimerEvent_h
#define TimerEvent_h

#include "Arduino.h"

class TimerEvent {
  private:
    uint32_t _startTime;
    uint32_t _now;
    bool _enabled;
    bool _actuated;

  public:
    TimerEvent() {};
    void setup() { 
      #ifdef DEBUG
      Serial.println("TimerEvent::setup()"); 
      #endif
      restart();
      disable();
    };

    void disable() {
      #ifdef DEBUG
      Serial.println("TimerEvent::disable()"); 
      #endif
      _enabled = false;
    };
    void enable() {
      #ifdef DEBUG
      Serial.println("TimerEvent::enable()");
      #endif
      _enabled = true;
    };
    
    void restart(uint32_t now = millis()) {
      #ifdef DEBUG
      Serial.println("TimerEvent::restart()");
      #endif
      _startTime = now;
      _actuated = false;
      enable();
    };

    void loop(uint32_t now = millis()) { _now = now; };

    void onTimer(uint32_t duration, void(*callback)(void *), void *obj = (void *)0) {
      if(!_enabled || duration == 0) return;
      
      if( _now >= _startTime + duration) {
        if(!_actuated) {
          _actuated = true;
          callback(obj);
        }
        return;          
      } // else
      _actuated = false;
  }
};
#endif
