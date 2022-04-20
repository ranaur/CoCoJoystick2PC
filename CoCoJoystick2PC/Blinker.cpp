#include "Blinker.h"

void Blinker::play(const uint16_t *playTimes, int repeat = -1)
{
  /*debug("Blinker::play() ");
  debugvar("repeat = ", repeat);
  debugvar("playTimes = ", (int)playTimes);
  debugln("");*/
  
  _playTimes = playTimes;
  _repeat = repeat;
  on();
}
 
void Blinker::loop(uint32_t now = millis())
{
  /*debugvar("start = ", _start);
  debugvar(" now = ", now);
  debugvar(" repeat = ", _repeat);
  debugvar(" position = ", _position);
  debugln("");*/

  if(_repeat == 0) return; // it is not playing

  if(now > _nextFade) {
    _fade(_intensityStep * (_turningOn ? 1 : -1));
    _nextFade += _timeStep;
  }
  
  if(now < _playUntil) return; // time to play next "time" has not yet arrived

  if(_playTimes[_position] == 0) { // ended the play
    if(_repeat < 0) { _reset(); return; } // infinite play
    _repeat--;
    
    if(_repeat == 0) { // turn off
      off();
    } else { // play again
      _reset();
    }
  }

  _playUntil += _playTimes[_position];
  if(_position % 2 == 0) {
    on();
    //debugln("ON");
  } else {
    //debugln("OFF");
    off();
  }

  _position++;
}
