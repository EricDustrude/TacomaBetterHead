#ifndef __CAMERA_STATE_H__
#define __CAMERA_STATE_H__

enum CameraOnOff {
  Off,
  On
};

enum CameraOption {
  
  // These are ordered  to match the dial input, left-center-right, 0-1-2.
  // That way it's simple integer division math to translate the analog value of the dial
  // to the 0, 1, or 2 of the camera selection.
  
  Front,
  TailGate,
  Trailer
};

#endif // ifndef __CAMERA_STATE_H__
