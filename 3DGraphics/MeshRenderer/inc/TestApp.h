#pragma once
#include "DXApp.h"
#include "ImGuiImpl.h"

using namespace DirectX;

//Forward declarations
class Sprite;
class MeshRenderer;
class IEntity;

class GraphicsManager;
class InputManager;

class TestApp : public DXApp
{
public:
  TestApp(HINSTANCE hInstance);
  ~TestApp();

  virtual bool Init() override;
  virtual void Update(float dt)override;
  void Render(float dt)override;

  //System variables
  GraphicsManager* m_graphics;
  InputManager* m_input;
  
  //Test Variables
  IEntity* m_testG0, *m_testGO2;
  MeshRenderer* m_testMesh, *m_testMesh2;
  ImGuiImpl m_ImGuiImp;

  static char g_meshFile[500];

};
void __stdcall CallbackChangeModel(void *clientData);
void __stdcall CallbackReAddShaders(void *clientData);
void __stdcall CallbackSetAllCommonPropertiesToLights(void *clientData);
void __stdcall CallbackSetAllDifferentPropertiesToLights(void *clientData);
void __stdcall CallbackSetAllRandomPropertiesToLights(void *clientData);
