
#include <imgui.h>
#include <ImGuiImpl.h>
#include <imgui_impl_dx11.h>

#define STB_IMAGE_IMPLEMENTATION
#include "STB/stb_image.h"

// NOTE: This file contains the code needed to render ImGui within an OpenGL 3
// context. This is very handy to have around, so feel free to reuse it in any
// other application you choose. Note that you may need to change the input
// bindings depending on what library you are using for windowing. Nevertheless,
// nothing in this file is necessary to understand to pass CS300, which is why
// it is provided in its entirety to you, with an overall lack of in-depth
// documentation. The OpenGL code used here is minimal and handy to know, but
// again, not critical for CS300.


std::deque<float> ImGuiImpl::m_FrameRateList = std::deque<float>();

ImGuiImpl::ImGuiImpl() : m_OpenAnimationWindow(true), m_DebugWindow(true), m_MeshWindow(true)
{
  for (unsigned i = 0; i < 100; ++i)
  {
    m_FrameRateList.push_front(0.0f);
  }
}

ImGuiImpl::~ImGuiImpl()
{
}

void ImGuiImpl::Initialize(void* hwnd, ID3D11Device* device, ID3D11DeviceContext* device_context)
{
  ImGui_ImplDX11_Init(hwnd, device, device_context);
}

void ImGuiImpl::Update(float dt)
{
  ImGui_ImplDX11_NewFrame();

  UpdateFramerateList(dt);

  ImGui::ShowTestWindow(&m_OpenAnimationWindow);
}

void ImGuiImpl::Render()
{
  ImGui::Render();
}

void ImGuiImpl::Cleanup()
{
  ImGui_ImplDX11_Shutdown();
}

const std::deque<float>& ImGuiImpl::GetFrameRateList(void)
{
  return m_FrameRateList;
}

void ImGuiImpl::UpdateFramerateList(float dt)
{
  m_FrameRateList.push_back(dt);
  m_FrameRateList.pop_front();
}
