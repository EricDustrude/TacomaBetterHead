


//=========================================================================== Includes
#include "CameraState.h"
#include "StateSwitch.h"
#include "Debug.h"

//=========================================================================== Defines


#define MIN_CAMERA_SIGNAL_LEVEL 5

// Pin in assignments
#define CAMERA_SIGNAL_FRONT A2
#define CAMERA_SIGNAL_TAILGATE A1
#define CAMERA_SIGNAL_TRAILER A0

#define CAMERA_ON_OFF 4
#define CAMERA_SELECT A5
#define PWM_DASH_BRIGHT A3
#define REV_IN 2

// Pin out assignments
#define CAMERA_SELECT_FRONT 13
#define CAMERA_SELECT_TAILGATE 12
#define CAMERA_SELECT_TRAILER 11

#define HEAD_DIM 9
#define REV_OUT 10

#define LIGHT 1
#define DARK 0
#define UNDEFINED -1
#define CENTER 1000
#define PADDING 50

//=========================================================================== Global variables
int prevPwmValue;
int displayMode;

DigitalStateSwitch userCameraOnButton(CAMERA_ON_OFF);
DigitalStateSwitch reverseSelect(REV_IN);

AnalogStateSwitch cameraSelector(CAMERA_SELECT, 17, 475, 3);

CameraOption prevCameraOption = TailGate;
CameraOnOff prevCameraOnOff = Off;

bool userCameraOnOffSelected = false;
bool reverseEngaged = false;
unsigned long pwmReadMillis = 0;
//=========================================================================== Functions
int mapCameraOptionToSelectPin(CameraOption cameraOption) {
  switch (cameraOption) {
    case Front: return CAMERA_SELECT_FRONT;
    case Trailer: return CAMERA_SELECT_TRAILER;
  }

  return CAMERA_SELECT_TAILGATE;
}

CameraOption mapDialToCameraOption(int dial) {

  return static_cast<CameraOption>(dial);
}

bool hasCameraSignal(int cameraPin) {
  return (MIN_CAMERA_SIGNAL_LEVEL <= analogRead(cameraPin));
}

CameraOption getAvailableBackupCamera() {

    return hasCameraSignal(CAMERA_SIGNAL_TRAILER)
      ? Trailer
      : TailGate;
}

void selectCamera(CameraOption cameraOption, CameraOnOff cameraOnOff) {
  
  int cameraSelectPin = mapCameraOptionToSelectPin(cameraOption);
  // Gurrantee we never have more than one video signal on at a time, turn off all camera inputs...
  digitalWrite(CAMERA_SELECT_FRONT, LOW);
  digitalWrite(CAMERA_SELECT_TAILGATE, LOW);
  digitalWrite(CAMERA_SELECT_TRAILER, LOW);
  
  digitalWrite(cameraSelectPin, On == cameraOnOff ? HIGH : LOW);
}

void setCameraDisplay(CameraOption cameraOption, CameraOnOff cameraOnOff) {

  if (prevCameraOption != cameraOption || prevCameraOnOff != cameraOnOff) {
    DebugPrint("setCameraDisplay("); DebugPrint(cameraOption); DebugPrint(", "); DebugPrint(cameraOnOff); DebugPrintln(")");

    selectCamera(cameraOption, cameraOnOff);

    if (prevCameraOnOff != cameraOnOff) {

      DebugPrint("Switching monitor "); DebugPrintln(On == cameraOnOff ? "On" : "Off");
      
      digitalWrite(REV_OUT, On == cameraOnOff ? HIGH : LOW);
    }
    
    prevCameraOption = cameraOption;
    prevCameraOnOff = cameraOnOff;
  }
}

void readDashBrightnessAndSetDayNightMode() {

  if (0 == pwmReadMillis) {
    pwmReadMillis = millis();

    return;
  }

  if (millis() - pwmReadMillis < 500) {
    return;
  }

  pwmReadMillis = 0;
  int pwmValue = pulseIn(PWM_DASH_BRIGHT, HIGH);
  DebugPrint("PWM: "); DebugPrintln(pwmValue);
  
  if (UNDEFINED != prevPwmValue) {
    if (prevPwmValue < pwmValue) {
      if (LIGHT != displayMode) {
        if (CENTER + PADDING < pwmValue) {
          // Switch ON
          displayMode = LIGHT;
          
          digitalWrite(HEAD_DIM, HIGH);
          
          DebugPrintln("Brightness HIGH");
        }
      }
    }
    else {
      if (LIGHT == displayMode) {
        if (pwmValue < CENTER - PADDING) {
          // Switch OFF
          displayMode = DARK;

          digitalWrite(HEAD_DIM, LOW);

          DebugPrintln("Brightness LOW");
        }
      }
    }
  }
  
  prevPwmValue = pwmValue;
}

void setup() {

  DebugBegin(57600);
  DebugPrintln("Setup");

  prevPwmValue = UNDEFINED;
  displayMode = LIGHT;

  // Set output pins
  pinMode(CAMERA_SELECT_FRONT, OUTPUT);
  digitalWrite(CAMERA_SELECT_FRONT, LOW);

  pinMode(CAMERA_SELECT_TAILGATE, OUTPUT);
  digitalWrite(CAMERA_SELECT_TAILGATE, LOW);

  pinMode(CAMERA_SELECT_TRAILER, OUTPUT);
  digitalWrite(CAMERA_SELECT_TRAILER, LOW);
  
  pinMode(HEAD_DIM, OUTPUT);
  digitalWrite(HEAD_DIM, LOW);

  pinMode(REV_OUT, OUTPUT);
  digitalWrite(REV_OUT, LOW);

  userCameraOnButton.initialize();
  reverseSelect.initialize();
  prevCameraOption = mapDialToCameraOption(cameraSelector.initialize());
  
  // delay(10);

  DebugPrintln("Setup complete");
}

void loop() {

  readDashBrightnessAndSetDayNightMode();

  CameraOption cameraOption = prevCameraOption;
  CameraOnOff cameraOnOff = prevCameraOnOff;

  cameraSelector.readState();
  reverseSelect.readState();
  userCameraOnButton.readState();

  if (cameraSelector.getStateChanged()) {

    int buttonState = cameraSelector.getState();

    cameraOption = mapDialToCameraOption(buttonState);

    DebugPrint("Camera selection change: "); DebugPrintln(cameraOption);
  }
  
  if (reverseSelect.getStateChanged()) {

    // Reverse is a "button" that stays in the state selected...
    int reverseState = reverseSelect.getState();
    if (HIGH == reverseState) {
      cameraOnOff = On;
      reverseEngaged = true;
    }
    else {
      reverseEngaged = false;
    }
    DebugPrint("Reverse engaged "); DebugPrintln(reverseEngaged);

    // Was the camera turned on by the user before going into reverse?
    // If so, don't switch it off here, only to have it come on again in the
    // next loop iteration...
    if (!reverseEngaged) {
      cameraOnOff = userCameraOnOffSelected ? On : Off;
    }
    else {
      // Reverse is being selected with shifter. So, let's
      // find the most appropriate camera to display.

      cameraOption = getAvailableBackupCamera();
    }
  }
  else if (userCameraOnButton.getStateChanged()) {

    // User button is momentary, so we need to toggle state whenever it's HIGH...
    int buttonState = userCameraOnButton.getState();
    if (HIGH == buttonState) {
      userCameraOnOffSelected = !userCameraOnOffSelected;

      cameraOnOff = userCameraOnOffSelected ? On : Off;
    }
  }

  setCameraDisplay(cameraOption, cameraOnOff);
}
