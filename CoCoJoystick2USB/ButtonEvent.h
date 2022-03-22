/*
  ButtonEvent.h - Library for reading joystick.
  Created by Gustavo Schoenaker
  Released into the public domain.
*/
#ifndef ButtonEvent_h
#define ButtonEvent_h

#include "Arduino.h"

#define null (void *)0

class ButtonEvent {
	public:
		ButtonEvent();
		void setup(int pin, bool pullup = true, int pressedState = LOW);
		void loop(uint32_t now = millis());
		void onPressed(void(*callback)(uint32_t, void *), void *obj = null);
		void onReleased(void(*callback)(uint32_t, void *), void *obj = null);
		void onPressedFor(void(*callback)(void *), uint32_t forMs, void *obj = null);
		void onReleasedFor(void(*callback)(void *), uint32_t forMs, void *obj = null);
		
		void printState();
	private:
		void printVar(char *name, uint32_t var);
			// Config vars
		int _pin;
		int _pressedState;
		int _pullup;

			// Transient vars
		uint32_t _now;
		int _state;

			// Debounce vars
		const int debounceDelay = 10;
		uint32_t _debounceTS;
		int _lastDebounceState;
		
			// Press/Release Vars
		int _lastStableState;
		bool _transition;
		uint32_t _pressedTS;
		uint32_t _releasedTS;

};

#endif
