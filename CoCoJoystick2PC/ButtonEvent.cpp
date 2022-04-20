#include "config.h"
#include "ButtonEvent.h"

ButtonEvent::ButtonEvent() {
	_pin = -1;
}

void ButtonEvent::printState() {
	debugln("ButtonEvent:");
	
	debug("    Config:");
	debugvar("  _pin = ", _pin );
	debugvar("  _pressedState = ", _pressedState );
	debugvar("  _pullup = ", _pullup );
  debugln("");
	
	debug("    Debounce:");
	debugvar("  _lastDebounceState = ", _lastDebounceState );
	debugvar("  _debounceTS = ", _debounceTS );
	debugvar("  (diff) = ", _now - _debounceTS );
	debugvar("  debounceDelay = ", debounceDelay );
  debugln("");

	debug("    Press/Release:");
	debugvar("  _pressedTS = ", _pressedTS );
	debugvar("  _releasedTS = ", _releasedTS );
	debugvar("  _lastStableState = ", _lastStableState );
  debugln("");

	debug("    Transient:");
	debugvar("  _now = ", _now );
	debugvar("  _state = ", _state );
  debugvar("  _transition = ", _transition );
 
  debugln("");
}

void ButtonEvent::setup(int pin, bool pullup = true, int pressedState = LOW) {
	_pin = pin;
	_pressedState = pressedState;
	_pullup = pullup;

	pinMode(_pin, _pullup ? INPUT_PULLUP : INPUT);
	
	_lastDebounceState = _releasedState();
	_debounceTS = millis();

  _pressedTS = _debounceTS;
  _releasedTS = _debounceTS;
  _lastStableState = _releasedState();
}

void ButtonEvent::loop(uint32_t now = millis()) {
	if(_pin == -1) { return; } // not initialized should run setup()

	_now = now;

  _transition = false;
	_state = digitalRead(_pin);
	if(_state != _lastDebounceState) {
		_lastDebounceState = _state;
		_debounceTS = _now;
	}

  if(_now < _debounceTS + debounceDelay) {
    debugln("debounce parole");
    _debounced = false;
    return; // in debounce parole
  }
  _debounced = true;
	
	if(_lastStableState != _state) {
    _transition = true;
    debugln("_transition = true;");
		_lastStableState = _state;
		if(_state == _pressedState) {
      debugln("_pressedTS = _now;");
			_pressedTS = _debounceTS;
		} else {
      debugln("_releasedTS = _now;");
			_releasedTS = _debounceTS;
		}
	} //else { debugln("same state"); }
}

void ButtonEvent::onPressed(void(*callback)(uint32_t, void *), void *obj = null) {
	if(!_debounced) return;
	if(_state == _pressedState && _transition) {
		callback(_now - _releasedTS, obj);
	}
}

void ButtonEvent::onReleased(void(*callback)(uint32_t, void *), void *obj = null) {
  if(!_debounced) return;
	if(_state != _pressedState && _transition) {
		callback(_now - _pressedTS, obj);
	}
}

void ButtonEvent::onPressedFor(uint32_t forMs, void(*callback)(void *), bool &actuated, void *obj = null) {
  if(!_debounced) return;
	if(_state == _pressedState) {
	  if((_now - _pressedTS) > forMs) {
      printState();
      if(!actuated) {
        /*debugvar("actuated = ", actuated);
        debugvar(" now = ", _now);
        debugvar(" _pressedTS = ", _pressedTS);
        debugvar(" forMs = ", forMs);
        debugln("");*/
        actuated = true;
        callback(obj);
      }
      return;
  	} //else { Serial.println("Not yet"); }
	}
	actuated = false;
}

void ButtonEvent::onReleasedFor(uint32_t forMs, void(*callback)(void *), bool &actuated, void *obj = null) {
  if(!_debounced) return;
	if(_state != _pressedState) {
	  if(_now - _releasedTS > forMs) {
      if(!actuated) {
        actuated = true;
        callback(obj);
      }
      return;
	  }
	}
	actuated = false;
}
