#include"DXUtil.h"
#include "InputManager.h"

InputManager::InputManager() :ISystem(ST_Input, "Input"), m_directInput(nullptr), m_windowHandle(0)
{
}

InputManager::~InputManager()
{
}

void InputManager::AddComponent(const Component comp)
{
}

void InputManager::RemoveComponent(const Component comp)
{
}

InputManager* InputManager::GetInstance()
{
  if (m_instance == nullptr)
  {
    return m_instance = new InputManager();
  }

  return m_instance;
}

InputManager** InputManager::GetInstanceRef()
{
  return &m_instance;
}

bool InputManager::Initialize()
{
  HR(DirectInput8Create(m_hAppInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
    (void**)&m_directInput, NULL));

  m_keyboard.m_directInput = m_directInput;
  m_mouse.m_directInput = m_directInput;

  m_keyboard.m_windowHandle = m_windowHandle;
  m_mouse.m_windowHandle = m_windowHandle;

  m_keyboard.Initialize();
  m_mouse.Initialize();

  return true;
}

void InputManager::Update(float dt)
{
  m_keyboard.Update(dt);
  m_mouse.Update(dt);
}

void InputManager::ShutDown()
{
  if (m_directInput)
    m_directInput->Release();
}

void InputManager::SetInstance(HINSTANCE instance)
{
  m_hAppInstance = instance;
}

void InputManager::SetHandle(HWND handle)
{
  m_windowHandle = handle;
}

InputManager* InputManager::m_instance = nullptr;
