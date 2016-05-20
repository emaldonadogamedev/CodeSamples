#include "Keyboard.h"
#include "DXUtil.h"

Keyboard::Keyboard(LPDIRECTINPUT8 directInput) :m_directInput(directInput), m_device(nullptr)
{
}


Keyboard::~Keyboard()
{
  if (m_device != nullptr)
  {
    m_device->Unacquire();
    m_device->Release();
    m_device = nullptr;
  }
}

const unsigned char* const Keyboard::CurrentState() const
{
  return m_currentState;
}

const unsigned char* const Keyboard::LastState() const
{
  return m_lastState;
}

void Keyboard::Initialize()
{
  HRESULT r;
  HR(r = m_directInput->CreateDevice(GUID_SysKeyboard, &m_device, nullptr));

  HR(r = m_device->SetDataFormat(&c_dfDIKeyboard));

  HR(r = m_device->SetCooperativeLevel(m_windowHandle, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE));

  m_device->Acquire();
}

void Keyboard::Update(const float dt)
{
  if (m_device)
  {
    m_device->Acquire();
    m_device->GetDeviceState(sizeof(m_currentState), (LPVOID)&m_currentState);

    memcpy(m_lastState, m_currentState, sizeof(m_currentState));
    //if (FAILED(m_device->GetDeviceState(sizeof(m_currentState), (LPVOID)m_currentState)))
    //{
    //  // Try to reaqcuire the device
    //  if (SUCCEEDED(m_device->Acquire()))
    //  {
    //    m_device->GetDeviceState(sizeof(m_currentState), (LPVOID)m_currentState);
    //  }
    //}
  }
}

bool Keyboard::IsKeyUp(unsigned char key) const
{
  return (m_currentState[key] & 0x80) == 0;
}

bool Keyboard::IsKeyDown(unsigned char key) const
{
  return m_currentState[key] & 0x80 ;
}

bool Keyboard::WasKeyUp(unsigned char key) const
{
  return (m_lastState[key] & 0x80) == 0;
}

bool Keyboard::WasKeyDown(unsigned char key) const
{
  return (m_lastState[key] & 0x80) != 0;
}

bool Keyboard::WasKeyPressedThisFrame(unsigned char key) const
{
  return (IsKeyDown(key) && WasKeyUp(key));
}

bool Keyboard::WasKeyReleasedThisFrame(unsigned char key) const
{
  return (IsKeyUp(key) && WasKeyDown(key));
}

bool Keyboard::IsKeyHeldDown(unsigned char key) const
{
  return (IsKeyDown(key) && WasKeyDown(key));
}

//const int Keyboard::KeyCount;