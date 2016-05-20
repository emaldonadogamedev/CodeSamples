#pragma once
#include <dinput.h>

class Keyboard
{
public:
  Keyboard(LPDIRECTINPUT8 directInput = 0);
  ~Keyboard();

  const unsigned char* const CurrentState() const;
  const unsigned char* const LastState() const;

  void Initialize();
  void Update(const float dt);

  bool IsKeyUp(unsigned char key) const;
  bool IsKeyDown(unsigned char key) const;
  bool WasKeyUp(unsigned char key) const;
  bool WasKeyDown(unsigned char key) const;
  bool WasKeyPressedThisFrame(unsigned char key) const;
  bool WasKeyReleasedThisFrame(unsigned char key) const;
  bool IsKeyHeldDown(unsigned char key) const;

  HWND m_windowHandle;
  LPDIRECTINPUT8 m_directInput;
private:
  static const int KeyCount = 256;
  IDirectInputDevice8* m_device;
  unsigned char m_currentState[KeyCount];
  unsigned char m_lastState[KeyCount];
};

