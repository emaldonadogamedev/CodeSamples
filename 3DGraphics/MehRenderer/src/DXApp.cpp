#include "DXApp.h"
#include "GraphicsManager.h"
#include "InputManager.h"
#include "AntTweakBar/include/AntTweakBar.h"

namespace
{
  //Used to forward messages to user-defined proc function
  DXApp* g_App = NULL;
}

#ifdef _DEBUG
#define r 0
#endif // _DEBUG


LRESULT CALLBACK MainWndproc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  if (g_App)
    return g_App->MsgProc(hwnd, msg, wParam, lParam);

  else
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

DXApp::DXApp(HINSTANCE hInstance)
{
  GraphicsManager::GetInstance()->SetInstance(hInstance);
  InputManager::GetInstance()->SetInstance(hInstance);
  g_App = this;

  //DXApp attributes default value
  m_AppPaused = false;
  m_Minimized = false;
  m_Maximized = false;
  m_Resizing = false;
  m_4xMsaaQuality = 0;
  m_Enable4xMsaa = false;
}

DXApp::~DXApp()
{
}

bool DXApp::Init()
{
  //Prepare graphics
  GraphicsManager::GetInstance()->SetMessageProc(MainWndproc);
  
  if (!GraphicsManager::GetInstance()->Initialize())
  {
    return false;
  }

  //Prepare input
  InputManager::GetInstance()->SetHandle( 
    GraphicsManager::GetInstance()->GetWindowHandle());

  if (!InputManager::GetInstance()->Initialize())
  {
    return false;
  }

  return true;
}

int DXApp::Run()
{
  //Main Message loop
  MSG msg = { 0 };

  m_Timer.Reset();

  while (WM_QUIT != msg.message)
  {
    if (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
    else
    {
      m_Timer.Tick();
      const float dt = m_Timer.DeltaTime();
      if(!m_AppPaused)
      {
        //Update
        Update(dt);

        //Render
        Render(dt);
      }
        
      Sleep(1);
    }
  }

  //Shutdown graphics
  GraphicsManager::GetInstance()->ShutDown();
  if (GraphicsManager::GetInstance())
  {
    _aligned_free(*GraphicsManager::GetInstanceRef());
  }
  
  //Shutdown input
  InputManager::GetInstance()->ShutDown();
  if (InputManager::GetInstance())
    delete InputManager::GetInstance();

  return static_cast<int>(msg.wParam);
}

LRESULT DXApp::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  //Send event message to AntTweakBar
  if (TwEventWin(hwnd, msg, wParam, lParam))
    return 0; // Event has been handled by AntTweakBar

  switch (msg)
  {
  case WM_SIZE:
    {
      int width = ((int)(short)LOWORD(lParam));
      int height = ((int)(short)HIWORD(lParam));
    }
    break;

      //////////////////////////////////////////////////////////////////////////
      //HANDLE KEYBOARD INPUT
  case WM_KEYDOWN:
      switch (wParam)
      {
      case VK_ESCAPE:
        PostQuitMessage(0);
        return 0;
        break;
      }
    break;
      //END HANDLE KEYBOARD INPUT
      //////////////////////////////////////////////////////////////////////////

  case WM_DESTROY:
    PostQuitMessage(0);
    return 0;

  default:
    return DefWindowProc(hwnd, msg, wParam, lParam);
  }

  return 0;
}


