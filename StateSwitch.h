#ifndef __STATESWITCH_H__
#define __STATESWITCH_H__

#include <Arduino.h>
#include "Debug.h"

class StateSwitch {
  private:
    const int NOT_SET = -1;
    const unsigned long DEBOUNCE_DELAY = 10;
  protected:
    int _pin;
    int _pinState;
    bool _stateChanged = false;
    int _prevValue;
    unsigned long _prevDebounceMillis = 0;

    // Overload these...
    virtual int getPinValue() = 0;
    virtual void readState() = 0;
    
    void updateValue(int newValue) {

      if (NOT_SET == _prevValue) {

        _pinState = newValue;
        _stateChanged = true;
      }
      else if (_prevValue != newValue) {
        _prevDebounceMillis = millis();
      }
      else if (DEBOUNCE_DELAY < (millis() - _prevDebounceMillis)) {

        if (newValue != _pinState) {
          _pinState = newValue;
          _stateChanged = true;
        }
      }

      _prevValue = newValue;
    }
    
  public:
    StateSwitch(int pin) {
      _pin = pin;
    }

    int getState() {
      _stateChanged = false;
      return _pinState;
    }
    
    bool getStateChanged() {
      return _stateChanged;
    }
    
    virtual int initialize() { 
      _prevValue = NOT_SET;

      return getPinValue();
    }
};

class DigitalStateSwitch : public StateSwitch {
  protected:
    int getPinValue() {
      int result = digitalRead(_pin);

      return result;
    }
    
  public:
    DigitalStateSwitch(int pin)
      : StateSwitch(pin) {
    }
    
    virtual int initialize() {
      
      pinMode(_pin, INPUT); 

      return StateSwitch::initialize();
    }
    
    virtual void readState() {
      updateValue(getPinValue());
    }
};

class AnalogStateSwitch : public StateSwitch {
  private:
    int _lowestExpectedReading;
    int _readingRange;
    int _divisor;
    int _divisions;

  protected:
    int getPinValue() {
      
      int pinValue = analogRead(_pin);
      int calculatedValue = (pinValue - _lowestExpectedReading) / _divisor;
      // Ensure we don't go below 0, or above divisions...
      calculatedValue = max(0, calculatedValue);
      calculatedValue = min(calculatedValue, (_divisions - 1));

      return calculatedValue;
    }
    
  public:
    AnalogStateSwitch(int pin, int lowestExpectedReading, int highestExpectedReading, int divisions)
      : StateSwitch(pin) {
      _lowestExpectedReading = lowestExpectedReading;
      _readingRange = highestExpectedReading - lowestExpectedReading;
      _divisor = _readingRange / divisions;
      _divisions = divisions;
    }

    virtual void readState() {
      
      updateValue(getPinValue());
    }
};

#endif // __STATESWITCH_H__
