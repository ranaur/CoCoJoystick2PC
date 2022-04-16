/*
  StateButtonEvent.h - Library for reading joystick.
  Created by Gustavo Schoenaker
  Released into the public domain.
*/
#ifndef StateButtonEvent_h
#define StateButtonEvent_h

#include "ButtonEvent.h"

#define null (void *)0

typedef unsigned int state_t;

class StateButtonEvent : public ButtonEvent {
  public:
    StateButtonEvent(state_t numberOfStates, state_t defaultState = 0);
    //void setEEPROMAddress(int address) { _EEPROMAdress = address };
    void onStateChange(void(*callback)(state_t, void *), void *obj = null);
    
    //void save();
    //void load();
    void reset() { setState(_defaultState); }
    
    void setState(state_t state) { if(_state != state % _numberOfStates) { _state = state % _numberOfStates; _changed = true; } }
    state_t getState() { return _state; }
    void skipState(state_t nJumps = 1) { setState(getState() + nJumps); }
    void nextState() { skipState(1); }

  private:
      // Config vars
    state_t _state;
    state_t _defaultState;
    state_t _numberOfStates;
    bool _changed;
    //int _EEPROMAdress;

    static void onButtonPressCallback(uint32_t dummy, void *obj);
};

#endif
