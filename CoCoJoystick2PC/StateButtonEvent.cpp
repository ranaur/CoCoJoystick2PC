#include "config.h"
#include "StateButtonEvent.h"

StateButtonEvent::StateButtonEvent(state_t numberOfStates, state_t defaultState = 0) : ButtonEvent() {
  _numberOfStates = numberOfStates;
  _defaultState = defaultState;
  reset();
  _changed = false;
  //setEEPROMAddress(-1);
}

static void StateButtonEvent::onButtonPressCallback(uint32_t dummy, void *obj) {
  debugln("onButtonPressCallback(...)");
  StateButtonEvent *me = (StateButtonEvent *)obj;
  me->nextState();
}

void StateButtonEvent::onStateChange(void(*callback)(state_t, void *), void *obj = null) {
  onPressed(onButtonPressCallback, this);

  if(_changed) {
    _changed = false;
    callback(getState(), obj);
  }
}
