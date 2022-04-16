#include "StateButtonEvent.h"

StateButtonEvent::StateButtonEvent(state_t numberOfStates, state_t defaultState = 0) {
  _numberOfStates = numberOfStates;
  _defaultState = defaultState;
  reset();
  //setEEPROMAddress(-1);
}

static void StateButtonEvent::onButtonPressCallback(uint32_t dummy, void *obj) {
  StateButtonEvent *me = (StateButtonEvent *)obj;
  me->nextState();
}

void StateButtonEvent::onStateChange(void(*callback)(state_t, void *), void *obj = null) {
  onPressed(onButtonPressCallback, this);

  if(changed) {
    changed = false;
    callback(getState(), obj);
  }
}
