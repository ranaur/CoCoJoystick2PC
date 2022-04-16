#include "StateButtonEvent.h"

StateButtonEvent::StateButtonEvent(state_t numberOfStates, state_t defaultState = 0) {
  _numberOfStates = numberOfStates;
  _defaultState = defaultState;
  reset();
  setEEPROMAddress(-1);
}

void StateButtonEvent::onButtonPressCallback(uint32_t dummy, void *obj) {
  obj->nextState();
}

void StateButtonEvent::onStateChange(void(*callback)(state_t, void *), void *obj = null) {
  struct payload p;
  this.onPressed(onButtonPressCallback, this);

  if(this.changed) {
    this.changed = false;
    callback(this.state, obj);
  }
}
