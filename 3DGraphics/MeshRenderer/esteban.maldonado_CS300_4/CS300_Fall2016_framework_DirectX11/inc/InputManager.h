#pragma once

#include "Keyboard.h"
#include "Mouse.h"

class InputManager : ISystem<>
{
public:
  ~InputManager();
  
  static InputManager* GetInstance();
  static InputManager** GetInstanceRef();

  //Inherited methods
  virtual bool Initialize() override;
  virtual void Update(float dt) override;
  virtual void ShutDown() override;
  virtual void AddComponent(const Component comp) override;
  virtual void RemoveComponent(const Component comp) override;

  void SetInstance(HINSTANCE instance);
  void SetHandle(HWND handle);

  Keyboard m_keyboard;
  Mouse m_mouse;
  
private:
  InputManager();

  LPDIRECTINPUT8 m_directInput;
  HINSTANCE m_hAppInstance;
  HWND m_windowHandle;



  static InputManager* m_instance;
};