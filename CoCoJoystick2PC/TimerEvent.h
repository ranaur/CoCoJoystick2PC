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
      debugfunction("TimerEvent::setup()"); 
      restart();
      disable();
    };

    void disable() {
      debugfunction("TimerEvent::disable()"); 
      _enabled = false;
    };
    void enable() {
      debugfunction("TimerEvent::enable()");
      _enabled = true;
    };
    
    void restart(uint32_t now = millis()) {
      debugfunction("TimerEvent::restart()");
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
