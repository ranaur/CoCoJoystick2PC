#ifndef CONFIG_H
#define CONFIG_H

#define DEBUG

#ifdef DEBUG
#define debugStart() { Serial.begin(9600); }
#define debug(s)   { Serial.print(F(s)); }
#define debugln(s)   { Serial.println(F(s)); }
#define debugvar(s,v)  { Serial.print(F(s)); Serial.print(v); }
#define debughex(s,v)  { Serial.print(s); Serial.print(v, HEX); }
#else
#define debugStart()
#define debug(s)
#define debugln(s)
#define debugvar(s,v)
#define debughex(s,v)
#endif

#ifdef DEBUG_FUNCTION
#define debugfunction(f) debugln(f)
#else
#define debugfunction(f)
#endif

#endif
