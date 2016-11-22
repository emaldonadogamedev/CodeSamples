#include "DXUtil.h"
#include "TestApp.h"
#include "GraphicsManager.h"
#include "TextureManager.h"
#include "InputManager.h"

#include "IEntity.h"
#include "MeshRenderer.h"

#include "AntTweakBar/include/AntTweakBar.h"

TestApp::TestApp(HINSTANCE hInstance) :DXApp(hInstance)
{
  //Get pointers to respective systems
  m_graphics = GraphicsManager::GetInstance();
  m_input = InputManager::GetInstance();

  //Prepare 1st object
  void* temp = _aligned_malloc(sizeof(IEntity), 16);
  m_testG0 = new (temp) IEntity();
  m_testG0->m_Transform.m_Position.z = 0.0f;
  m_testMesh = new MeshRenderer(m_testG0, "box.obj");
  m_testMesh->m_phongMaterial->UseTexture = false;
  m_graphics->AddComponent(m_testMesh);
  m_testG0->AddComponent(m_testMesh);

  //Prepare 2nd object
  temp = _aligned_malloc(sizeof(IEntity), 16);
  m_testGO2 = new (temp)IEntity();
  m_testMesh2 = new MeshRenderer(m_testGO2, "box.obj");
  m_testGO2->m_Transform.m_Position.y = -2.0f;
  m_testGO2->m_Transform.m_Scale.x = m_testGO2->m_Transform.m_Scale.z = 3.0f;
  m_testGO2->m_Transform.m_Scale.y = .20f;
  m_testMesh2->m_phongMaterial->UseTexture = false;
  m_testMesh2->m_IsActive = true;

  m_graphics->AddComponent(m_testMesh2);
  m_testGO2->AddComponent(m_testMesh2);
}

TestApp::~TestApp()
{
}

bool TestApp::Init()
{
  if ( DXApp::Init() )
  {
    //load the default textures on to GPU memory
    m_testMesh->SetTexture(TextureManager::getTexture(DEFAULT_TEXTURE));
    m_testMesh->SetSpecTexture(TextureManager::getTexture(DEFAULT_SPEC_TEXTURE));
    m_testMesh->SetNormalTexture(TextureManager::getTexture(std::string(NORMAL_MAP) + DEFAULT_SPEC_TEXTURE));// std::wstring(NORMAL_MAP) + std::wstring(DEFAULT_NORMAL_TEXTURE)));

    TwBar* model_bar = m_graphics->m_modelPropertiesBar;
    TwBar* graphics_bar = m_graphics->m_graphicsSettingsBar;
    TwBar* lights_bar = m_graphics->m_lightPropertiesBar;
    TwBar* extraLights_bar = m_graphics->m_extraLightPropertiesBar;

    const int barSize[2] = { 300, 300 };
    TwSetParam(model_bar, NULL, "size", TW_PARAM_INT32, 2, barSize);

    //Graphics / Renderer Properties Bar
    const int graphicsBarSize[2] = { 300, 140 };
    TwSetParam(graphics_bar, NULL, "size", TW_PARAM_INT32, 2, graphicsBarSize);
    TwDefine("Graphics_Properties position='650 10'");
    TwAddVarRW(graphics_bar, "Raster State", TW_TYPE_INT32, &m_graphics->testRasterState, "min=0 max=3");
    TwAddVarRW(graphics_bar, "Clear Color", TW_TYPE_COLOR3F, &m_graphics->m_clearScreenColor, "min=0 max=3");
    TwAddVarRW(graphics_bar, "Draw Face Normals", TW_TYPE_BOOLCPP, &m_graphics->m_drawFaceNormals, "true='On' false='Off' ");
    TwAddVarRW(graphics_bar, "Draw Vertex Normals", TW_TYPE_BOOLCPP, &m_graphics->m_drawVertexNormals, "true='On' false='Off' ");
    TwAddButton(graphics_bar, "Re-Add Shaders (Pess Button)", CallbackReAddShaders, NULL, "");
    
    //Model properties / Game Object Bar
    TwAddVarRW(model_bar, "Model file: ex. [name].obj", TW_TYPE_CSSTRING(sizeof(g_meshFile)), g_meshFile, "group=Object");
    TwAddButton(model_bar, "Load Model(Press Button)", CallbackChangeModel, m_testMesh, "group=Object");
    TwAddVarRW(model_bar, "Translation", TW_TYPE_DIR3F, &m_testG0->m_Transform.m_Position, "group=Object step=0.001 ");
    TwAddVarRW(model_bar, "Rotation", TW_TYPE_DIR3F, &m_testG0->m_Transform.m_Rotation, "group=Object step=0.0001 min=-6.2831 max=6.2831 ");
      TwDefine(" Model_Properties/Rotation axisx=x axisy=y axisz=-z ");
    TwAddVarRW(model_bar, "Scale", TW_TYPE_DIR3F, &m_testG0->m_Transform.m_Scale, "group=Object step=0.001 ");
    
    //Model properties - Material
    TwAddVarRW(model_bar, "Use Blinn-Phong", TW_TYPE_BOOLCPP, &m_testMesh->m_phongMaterial->UseBlinn, "group=Material");
    TwAddVarRW(model_bar, "Use Texture", TW_TYPE_BOOLCPP, &m_testMesh->m_phongMaterial->UseTexture, "group=Material");
    TwAddVarRW(model_bar, "Use Normal Map", TW_TYPE_BOOLCPP, &m_testMesh->m_phongMaterial->UseNormalMap, "group=Normal_Map");
    TwAddVarRW(model_bar, "Render Normal Map", TW_TYPE_BOOLCPP, &m_graphics->m_constBuffer.renderNormalMap, "group=Normal_Map");
    TwAddVarRW(model_bar, "Render Tangents", TW_TYPE_BOOLCPP, &m_graphics->m_constBuffer.renderTangents, "group=Normal_Map");
    TwAddVarRW(model_bar, "Render BiTangents", TW_TYPE_BOOLCPP, &m_graphics->m_constBuffer.renderBiTangents, "group=Normal_Map");
    TwAddVarRW(model_bar, "Texture Type", TW_TYPE_INT32, &m_graphics->m_constBuffer.textureType, "group=Material min=0 max=2");
    TwAddVarRW(model_bar, "Ka", TW_TYPE_FLOAT, &m_testMesh->m_phongMaterial->Ka, "group=Material step=0.001 min=0 max=1 ");
    TwAddVarRW(model_bar, "Kd", TW_TYPE_FLOAT, &m_testMesh->m_phongMaterial->Kd, "group=Material step=0.001 min=0 max=1 ");
    TwAddVarRW(model_bar, "Ks", TW_TYPE_FLOAT, &m_testMesh->m_phongMaterial->Ks, "group=Material step=0.001 min=0 max=1 ");
    TwAddVarRW(model_bar, "Specular Power", TW_TYPE_FLOAT, &m_testMesh->m_phongMaterial->SpecularPower, "group=Material step=0.001 min=0 ");
    TwAddVarRW(model_bar, "Emissive", TW_TYPE_COLOR4F, &m_testMesh->m_phongMaterial->Emissive, "group=Material step=0.0001 min=0 max=1 ");
    TwDefine("Model_Properties/Material group=Object");

    //Light Properties Bar
    TwSetParam(lights_bar, NULL, "size", TW_PARAM_INT32, 2, barSize);
    TwDefine("Light_Properties position='15 360'");
    TwAddVarRW(lights_bar, "Draw Debug Light Spheres", TW_TYPE_BOOLCPP, &m_graphics->m_drawLightSpheres, "true='On' false='Off' ");
    TwAddVarRW(lights_bar, "Global Ambient", TW_TYPE_COLOR4F, &m_graphics->m_lightConstBuffer.GlobalAmbient, "step=0.001");
    
    TwAddVarRW(lights_bar, "Spin Lights", TW_TYPE_BOOLCPP, &m_graphics->m_spinLights, "true='On' false='Off' ");
    TwAddVarRW(lights_bar, "Spin Speed", TW_TYPE_FLOAT, &m_graphics->m_spinningSpeed, " step=0.001 min=0 max=2.0 ");
    TwAddVarRW(lights_bar, "Spin Radius", TW_TYPE_FLOAT, &m_graphics->m_spinRadius, " step=0.001 min=0=2.5 max=20.0 ");
    TwAddVarRW(lights_bar, "Sine wave Lights", TW_TYPE_BOOLCPP, &m_graphics->m_sineWavLights, "true='On' false='Off' ");
    TwAddVarRW(lights_bar, "Sine wave Height Scalar", TW_TYPE_FLOAT, &m_graphics->m_sineHeightScalar, " step=0.001 min=0 max=10.0 ");

#pragma region Individual light properties UI
    char buff[200] = { 0 }, name[100] = { 0 };
    for (int i = 0; i < MAX_LIGHTS; i++)
    {
      //Light properties #1
      sprintf_s(name, "Enabled %i", i + 1);
      sprintf_s(buff, "group=Light%i", i + 1);
      TwAddVarRW(lights_bar, name, TW_TYPE_BOOLCPP, &m_graphics->m_lightConstBuffer.Lights[i].m_Enabled, buff);

      sprintf_s(name, "Light Type %i", i + 1);
      sprintf_s(buff, "group=Light%i min=0 max=2 ", i + 1);
      TwAddVarRW(lights_bar, name, TW_TYPE_INT32, &m_graphics->m_lightConstBuffer.Lights[i].m_lightType, buff);

      sprintf_s(buff, "group=Light%i step=0.001 ", i + 1);

      TwAddVarRW(lights_bar, "Attenuation - C1", TW_TYPE_FLOAT, &m_graphics->m_lightConstBuffer.Lights[i].m_ConstantAttenuation, buff);
      TwAddVarRW(lights_bar, "Attenuation - C2", TW_TYPE_FLOAT, &m_graphics->m_lightConstBuffer.Lights[i].m_LinearAttenuation, buff);
      TwAddVarRW(lights_bar, "Attenuation - C3", TW_TYPE_FLOAT, &m_graphics->m_lightConstBuffer.Lights[i].m_QuadraticAttenuation, buff);

      sprintf_s(name, "Position %i", i + 1);
      TwAddVarRW(lights_bar, name, TW_TYPE_DIR3F, &m_graphics->m_lightConstBuffer.Lights[i].m_Position, buff);

      sprintf_s(buff, "group=Light%i step=0.001 min=-1 max=1", i + 1);
      sprintf_s(name, "Spotlight Dir %i", i + 1);// "group=Material step=0.001  "
      TwAddVarRW(lights_bar, name, TW_TYPE_DIR3F, &m_graphics->m_lightConstBuffer.Lights[i].m_Direction, buff);

      sprintf_s(buff, "group=Light%i step=0.001 ", i + 1);
      sprintf_s(name, "Spot Inner Angle %i", i + 1);
      TwAddVarRW(lights_bar, name, TW_TYPE_FLOAT, &m_graphics->m_lightConstBuffer.Lights[i].m_SpotAngleInner, buff);

      sprintf_s(name, "Spot Outer Angle %i", i + 1);
      TwAddVarRW(lights_bar, name, TW_TYPE_FLOAT, &m_graphics->m_lightConstBuffer.Lights[i].m_SpotAngleOuter, buff);

      sprintf_s(name, "Spot Falloff %i", i + 1);
      TwAddVarRW(lights_bar, name, TW_TYPE_FLOAT, &m_graphics->m_lightConstBuffer.Lights[i].m_SpotFallOff, buff);

      sprintf_s(name, "Ia %i", i + 1);
      TwAddVarRW(lights_bar, name, TW_TYPE_COLOR4F, &m_graphics->m_lightConstBuffer.Lights[i].m_Ia, buff);

      sprintf_s(name, "Id %i", i + 1);
      TwAddVarRW(lights_bar, name, TW_TYPE_COLOR4F, &m_graphics->m_lightConstBuffer.Lights[i].m_Id, buff);

      sprintf_s(name, "Is %i", i + 1);
      TwAddVarRW(lights_bar, name, TW_TYPE_COLOR4F, &m_graphics->m_lightConstBuffer.Lights[i].m_Is, buff);
    }

#pragma endregion

#pragma region Common Light Properties

    TwSetParam(extraLights_bar, NULL, "size", TW_PARAM_INT32, 2, barSize);
    TwDefine("Extra_Light_Properties position='650 300'");

    TwAddVarRW(extraLights_bar, "Spin Camera", TW_TYPE_BOOLCPP, &m_graphics->m_spinCamera, "");
    TwAddVarRW(extraLights_bar, "Spin Camera Speed", TW_TYPE_FLOAT, &m_graphics->m_spinCameraSpeed, "step=0.001 min=0 max=2.0");
    TwAddVarRW(extraLights_bar, "Spin Camera Radius", TW_TYPE_FLOAT, &m_graphics->m_spinCameraRadius, "step=0.001 min=2.5 max=20.0");

    TwAddButton(extraLights_bar, "Apply settings to lights(Press Button)", CallbackSetAllCommonPropertiesToLights, nullptr, "");

    sprintf_s(buff, "group=Extra_Common_Properties");
    TwAddVarRW(extraLights_bar, "Enabled", TW_TYPE_BOOLCPP, &m_graphics->m_CommonEnabled, buff);
    TwAddVarRW(extraLights_bar, "Common_Type", TW_TYPE_INT32, &m_graphics->m_CommonlightType, "group=Extra_Common_Properties min=0 max=2 ");
    
    sprintf_s(buff, "group=Extra_Common_Properties step=0.001 ");
    TwAddVarRW(lights_bar, "Attenuation - C1", TW_TYPE_FLOAT, &m_graphics->m_CommonConstantAttenuation, buff);
    TwAddVarRW(lights_bar, "Attenuation - C2", TW_TYPE_FLOAT, &m_graphics->m_CommonLinearAttenuation, buff);
    TwAddVarRW(lights_bar, "Attenuation - C3", TW_TYPE_FLOAT, &m_graphics->m_CommonQuadraticAttenuation, buff);

    sprintf_s(buff, "group=Extra_Common_Properties step=0.001 ");
    TwAddVarRW(lights_bar, "Spot Inner Angle", TW_TYPE_FLOAT, &m_graphics->m_CommonSpotAngleInner, buff);
    TwAddVarRW(lights_bar, "Spot Outer Angle", TW_TYPE_FLOAT, &m_graphics->m_CommonSpotAngleOuter, buff);
    TwAddVarRW(lights_bar, "Spot Falloff", TW_TYPE_FLOAT, &m_graphics->m_CommonSpotFallOff, buff);

    TwAddVarRW(extraLights_bar, "Ia", TW_TYPE_COLOR4F, &m_graphics->m_CommonIa, buff);
    TwAddVarRW(extraLights_bar, "Id", TW_TYPE_COLOR4F, &m_graphics->m_CommonId, buff);
    TwAddVarRW(extraLights_bar, "Is", TW_TYPE_COLOR4F, &m_graphics->m_CommonIs, buff);

    TwAddButton(extraLights_bar, "Apply Random settings to lights(Press Button)", CallbackSetAllRandomPropertiesToLights, nullptr, "");

#pragma endregion

    const float r = 2.5f;
    float i = 0;

    //place all of the lights around the origin...
    for (int j = 0;
      i <= DirectX::XM_2PI && j < MAX_LIGHTS;
      j++, i += (DirectX::XM_2PI / (float)MAX_LIGHTS))
    {
      m_graphics->m_lightConstBuffer.Lights[j].m_Position.x = r * cos(i);
      m_graphics->m_lightConstBuffer.Lights[j].m_Position.z = r * sin(i);
      m_graphics->m_lightConstBuffer.Lights[j].m_Direction.x = -m_graphics->m_lightConstBuffer.Lights[j].m_Position.x;
      m_graphics->m_lightConstBuffer.Lights[j].m_Direction.y = -m_graphics->m_lightConstBuffer.Lights[j].m_Position.y;
      m_graphics->m_lightConstBuffer.Lights[j].m_Direction.z = -m_graphics->m_lightConstBuffer.Lights[j].m_Position.z;
    }

    return true;
  }

  return false;
}

void TestApp::Update(float dt)
{
  m_input->Update(dt);
  m_graphics->Update(dt);

  Keyboard* keyboard = &InputManager::GetInstance()->m_keyboard;
  Mouse* mouse = &InputManager::GetInstance()->m_mouse;

}

void TestApp::Render(float dt)
{
  m_graphics->Render(dt);
}

char TestApp::g_meshFile[500] = { 0 };

void __stdcall CallbackChangeModel(void *clientData)
{
  if (clientData)
  { 
    MeshRenderer* temp = reinterpret_cast<MeshRenderer*>(clientData);
    temp->ChangeModel(TestApp::g_meshFile);
  }
}

void __stdcall CallbackReAddShaders(void *clientData)
{
  GraphicsManager::GetInstance()->ReAddShaders();
}

void __stdcall CallbackSetAllCommonPropertiesToLights(void *clientData)
{
  GraphicsManager::GetInstance()->SetAllLightsToCommon();
}

void __stdcall CallbackSetAllDifferentPropertiesToLights(void *clientData)
{
  GraphicsManager::GetInstance()->SetAllLightsToDifferent();
}

void __stdcall CallbackSetAllRandomPropertiesToLights(void *clientData)
{
  GraphicsManager::GetInstance()->SetAllLightsToRandom();
}
