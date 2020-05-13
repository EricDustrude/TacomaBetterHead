#ifndef __DEBUG_H__
#define __DEBUG_H__

// define DEBUG

#ifdef DEBUG
  #define DebugBegin(a) Serial.begin(a)
  #define DebugPrint(a) Serial.print(a)
  #define DebugPrintln(a) Serial.println(a)
#else
  #define DebugBegin(a) ;
  #define DebugPrint(a) ;
  #define DebugPrintln(a) ;
#endif

#endif // __DEBUG_H__
