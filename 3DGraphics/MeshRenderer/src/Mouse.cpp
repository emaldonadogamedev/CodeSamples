#include "Mouse.h"
#include "DXUtil.h"
#include <assert.h>

Mouse::Mouse(LPDIRECTINPUT8 directInput) :m_directInput(directInput)
{
  //assert(m_directInput != nullptr);
  ZeroMemory(&m_currentState, sizeof(DIMOUSESTATE));
  ZeroMemory(&m_lastState, sizeof(DIMOUSESTATE));
}

Mouse::~Mouse()
{
  if (m_device != nullptr)
  {
    m_device->Unacquire();
    m_device->Release();
    m_device = nullptr;
  }
}

LPDIMOUSESTATE Mouse::CurrentState()
{
  return &m_currentState;
}

LPDIMOUSESTATE Mouse::LastState()
{
  return &m_lastState;
}

void Mouse::Initialize()
{
  HR(m_directInput->CreateDevice(GUID_SysMouse, &m_device, nullptr));

  HR(m_device->SetDataFormat(&c_dfDIMouse));

  HR(m_device->SetCooperativeLevel(m_windowHandle, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE));

  m_device->Acquire();
}

void Mouse::Update(float dt)
{
  if (m_device)
  {
    m_device->Acquire();
    m_device->GetDeviceState(sizeof(m_currentState), &m_currentState);

    if ((m_currentState.lX != m_lastState.lX) || (m_currentState.lY != m_lastState.lY))
    {
      m_lastState = m_currentState;
    }

    //memcpy(&m_lastState, &m_currentState, sizeof(m_currentState));
    //if (FAILED(m_device->GetDeviceState(sizeof(m_currentState), &m_currentState)))
    //{
    //  // Try to reaqcuire the device
    //  if (SUCCEEDED(m_device->Acquire()))
    //  {
    //    m_device->GetDeviceState(sizeof(m_currentState), &m_currentState);
    //  }
    //}

    // Accumulate positions
    mX += m_currentState.lX;
    mY += m_currentState.lY;
    mWheel += m_currentState.lZ;
  }
}

long Mouse::X() const
{
  return mX;
}

long Mouse::Y() const
{
  return mY;
}

long Mouse::Wheel() const
{
  return mWheel;
}

bool Mouse::IsButtonUp(MouseButtons button) const
{
  return ((m_currentState.rgbButtons[button] & 0x80) == 0);
}

bool Mouse::IsButtonDown(MouseButtons button) const
{
  return ((m_currentState.rgbButtons[button] & 0x80) != 0);
}

bool Mouse::WasButtonUp(MouseButtons button) const
{
  return ((m_lastState.rgbButtons[button] & 0x80) == 0);
}

bool Mouse::WasButtonDown(MouseButtons button) const
{
  return ((m_lastState.rgbButtons[button] & 0x80) != 0);
}

bool Mouse::WasButtonPressedThisFrame(MouseButtons button) const
{
  return (IsButtonDown(button) && WasButtonUp(button));
}

bool Mouse::WasButtonReleasedThisFrame(MouseButtons button) const
{
  return (IsButtonUp(button) && WasButtonDown(button));
}

bool Mouse::IsButtonHeldDown(MouseButtons button) const
{
  return (IsButtonDown(button) && WasButtonDown(button));
}
