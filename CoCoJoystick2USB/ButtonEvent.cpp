#include "ButtonEvent.h"

ButtonEvent::ButtonEvent() {
	_pin = -1;
}

void ButtonEvent::printVar(char *name, uint32_t var) {
	Serial.print(name);
	Serial.print(" = ");
	Serial.print(var);
}

void ButtonEvent::printState() {
	Serial.println("ButtonEvent:");
	
	Serial.print("    Config:");
	printVar("  _pin", _pin );
	printVar("  _pressedState", _pressedState );
	printVar("  _pullup", _pullup );
  Serial.println("");
	
	Serial.print("    Debounce:");
	printVar("  _lastDebounceState", _lastDebounceState );
	printVar("  _debounceTS", _debounceTS );
	printVar("  (diff)", _now - _debounceTS );
	printVar("  debounceDelay", debounceDelay );
	Serial.println("");

	Serial.print("    Press/Release:");
	printVar("  _pressedTS", _pressedTS );
	printVar("  _releasedTS", _releasedTS );
	printVar("  _lastStableState", _lastStableState );
	Serial.println("");

	Serial.print("    Transient:");
	printVar("  _now", _now );
	printVar("  _state", _state );
	Serial.println("");
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
