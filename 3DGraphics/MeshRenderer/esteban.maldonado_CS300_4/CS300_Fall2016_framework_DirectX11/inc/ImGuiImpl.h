#pragma once

struct ID3D11Device;
struct ID3D11DeviceContext;

#include <deque>

// This class is responsible for managing the actual ImGui instance, including
// initialization, drawing it, cleaning up resources, and handling input
// interactions with FreeGLUT. You shouldn't have to change anything in this
// file or in ImGuiImpl.cpp. Defining new ImGui windows is completely
// independent of any functionality implemented here. This implementation is
// specific to OpenGL 3.3 using FreeGLUT for I/O. This code can easily be
// adapted to other versions of OpenGL or other input libraries.
class ImGuiImpl
{
public:
  ImGuiImpl();
  ~ImGuiImpl();

  void Initialize(void* hwnd, ID3D11Device* device, ID3D11DeviceContext* device_context);
  void Update(float dt);
  void Render();
  void Cleanup();

  static const std::deque<float>& GetFrameRateList(void);

private:
  void UpdateFramerateList(float dt);

  bool m_OpenAnimationWindow;
  bool m_DebugWindow;
  bool m_MeshWindow;

  static std::deque<float> ImGuiImpl::m_FrameRateList;
};