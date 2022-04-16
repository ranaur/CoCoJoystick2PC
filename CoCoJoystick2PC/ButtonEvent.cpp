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
  debugln("");
}

void ButtonEvent::setup(int pin, bool pullup = true, int pressedState = LOW) {
	_pin = pin;
	_pressedState = pressedState;
	_pullup = pullup;

	pinMode(_pin, _pullup ? INPUT_PULLUP : INPUT);
	
	_lastDebounceState = _pressedState == HIGH ? LOW : HIGH;
	_debounceTS = millis();
	
	_lastStableState;
	_pressedTS = _debounceTS;
	_releasedTS = _debounceTS;
}

void ButtonEvent::loop(uint32_t now = millis()) {
	if(_pin == -1) { return; } // not initialized should run setup()

	_now = now;

	_state = digitalRead(_pin);
	if(_state != _lastDebounceState) {
		_lastDebounceState = _state;
		_debounceTS = _now;
	}

	_transition = false;
	if(_now - _debounceTS > debounceDelay) {
		if(_lastStableState != _state) {
			_lastStableState = _state;
			_transition = true;
			if(_state == _pressedState) {
				_pressedTS = _now;
			} else {
				_releasedTS = _now;
			}
		}
	}
}

void ButtonEvent::onPressed(void(*callback)(uint32_t, void *), void *obj = null) {
	if(_state == _pressedState && _transition) {
		callback(_now - _releasedTS, obj);
	}
}

void ButtonEvent::onReleased(void(*callback)(uint32_t, void *), void *obj = null) {
	if(_state != _pressedState && _transition) {
		callback(_now - _pressedTS, obj);
	}
}

void ButtonEvent::onPressedFor(uint32_t forMs, void(*callback)(void *), bool &actuated, void *obj = null) {
	if(_state == _pressedState) {
	  if(_now - _pressedTS > forMs) {
      if(!actuated) {
        actuated = true;
        callback(obj);
      }
      return;
  	}
	}
	actuated = false;
}

void ButtonEvent::onReleasedFor(uint32_t forMs, void(*callback)(void *), bool &actuated, void *obj = null) {
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
