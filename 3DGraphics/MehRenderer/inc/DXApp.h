#pragma once
#define WIN32_LEAN_AND_MEAN

#include"DXUtil.h"

using namespace Memory;

class DXApp
{
public:
  DXApp(HINSTANCE hInstance);
  virtual ~DXApp();

  //Main Application Loop
  int Run();

  //Framework methods
  virtual bool Init();
  virtual void Update(float dt) = 0;
  virtual void Render(float dt) = 0;
  virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

protected:
  //DXApp Attributes
  bool m_AppPaused;
  bool m_Minimized;
  bool m_Maximized;
  bool m_Resizing;
  UINT m_4xMsaaQuality;
  bool m_Enable4xMsaa;
  GameTimer m_Timer;
};

