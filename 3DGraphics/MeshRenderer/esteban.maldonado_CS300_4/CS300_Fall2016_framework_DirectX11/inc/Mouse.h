#pragma once

#include <dinput.h>


enum MouseButtons
{
  MouseButtonsLeft = 0,
  MouseButtonsRight = 1,
  MouseButtonsMiddle = 2,
  MouseButtonsX1 = 3
};

class Mouse
{
public:
  Mouse(LPDIRECTINPUT8 directInput = 0);
  ~Mouse();

  LPDIMOUSESTATE CurrentState();
  LPDIMOUSESTATE LastState();

  void Initialize();
  void Update(float dt);

  long X() const;
  long Y() const;
  long Wheel() const;

  bool IsButtonUp(MouseButtons button) const;
  bool IsButtonDown(MouseButtons button) const;
  bool WasButtonUp(MouseButtons button) const;
  bool WasButtonDown(MouseButtons button) const;
  bool WasButtonPressedThisFrame(MouseButtons button) const;
  bool WasButtonReleasedThisFrame(MouseButtons button) const;
  bool IsButtonHeldDown(MouseButtons button) const;

  HWND m_windowHandle;
  LPDIRECTINPUT8 m_directInput;
private:
  
  IDirectInputDevice8* m_device;
  DIMOUSESTATE m_currentState;
  DIMOUSESTATE m_lastState;

  long mX;
  long mY;
  long mWheel;
};

