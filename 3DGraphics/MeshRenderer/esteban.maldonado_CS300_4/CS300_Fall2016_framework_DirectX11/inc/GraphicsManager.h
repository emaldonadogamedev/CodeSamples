#pragma once

#include "GFXTypes.h"
#include "DXUtil.h"

#define DEFAULT_WINDOW_WIDTH 1024
#define DEFAULT_WINDOW_HEIGHT 700
#define DEFAULT_TEXTURE "metal_roof_diff_512x512.tga"
#define DEFAULT_SPEC_TEXTURE "metal_roof_spec_512x512.tga"
#define DEFAULT_NORMAL_TEXTURE "metal_roof_spec_512x512.tga"

//forward declarations
class VertexShader;
class PixelShader;
class ImGuiImpl;
class CubeMap;
class DynamicCubeMap;
typedef struct CTwBar TwBar;


enum RasterStates : int
{
  RS_SOLID_DEFAULT,
  RS_WIREFRAME,
  RS_SOLID_CULL_BACK,
  RS_SOLID_CULL_FRONT
};

__declspec(align(16)) class GraphicsManager : public ISystem<RenderComp>
{
public:
  ~GraphicsManager();

  //Getters
  static GraphicsManager* GetInstance();
  static GraphicsManager** GetInstanceRef();

  ID3D11Device* GetDevice()const;
  ID3D11DeviceContext* GetContext()const;
  IDXGISwapChain* GetSwapChain()const;
  ID3D11RenderTargetView** GetRenderTargetView();
  D3D_PRIMITIVE_TOPOLOGY& GetPrimitiveTopology();
  HWND GetWindowHandle();

  //Ant Tweak Bar Getters
  TwBar* GetMyAntTweakBar();

  CubeMap* m_cubeMap;
  DynamicCubeMap* m_dynCubeMap;
  IRendererComponent* m_dynCubeMapMesh;

  //Inherited methods
  virtual bool Initialize() override;
  virtual void Update(float dt) override;
  virtual void ShutDown() override;
  virtual void Render(float dt); //Render scene
  virtual void AddComponent(const RenderComp comp) override;
  virtual void RemoveComponent(const RenderComp comp) override;

  //Setters
  void SetInstance(const HINSTANCE hInstance);
  void SetMessageProc(const WNDPROC& proc);
  void SetPrimitiveTopology(int pt);
  void SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY pt);

  //Material Setters
  void SetMaterialProperties(MaterialProperties& mat);
  
  //Light Setters
  void UpdateLightCommonProperties(XMFLOAT4& cam_position, XMFLOAT4& fogColor);

  //3 different scenarios
  void SetAllLightsToCommon(); //#1
  void SetAllLightsToDifferent(); //#2
  void SetAllLightsToRandom(); // #3


  //Shader Setters
  void ReAddShaders();

  //Window Methods
  void ResizeWindow(int width = 0, int height = 0);

  XMVECTORF32 m_clearScreenColor;
private:
  GraphicsManager();
  static GraphicsManager* m_Instance;

  //Initialize WIN32 Window
  bool InitWindow();

  //Initialize direct3D
  bool InitDirect3D();
  bool InitAntTweakBar();
  bool InitImGui();
  bool InitConstantBuffer();
  bool InitLightConstantBuffer();
  bool InitMaterialConstantBuffer();
  bool InitTextureSamplerStates();
  void ResizeSwapChain();

  //Additional render functions
  void RenderSkyBox(float dt);
  void RenderLightSpheres();

  //Updte camera
  void UpdateCamera(float dt);

  //////////////////////////////////////////////////////////////////////////
  //WIN32 ATTRIBUTES
  HWND m_hAppWnd;
  HINSTANCE m_hAppInstance;
  WNDPROC m_MessageProc;
  UINT m_ClientWidth;
  UINT m_ClientHeight;
  std::string m_AppTitle;
  std::string m_MainWndCaption;
  DWORD m_WndStyle;

  //Directx Attributes
  ID3D11Device* m_pDevice;
  ID3D11DeviceContext* m_pImmediateContext;
  IDXGISwapChain* m_pSwapChain;
  ID3D11Texture2D* m_DepthStencilBuffer;
  ID3D11RenderTargetView* m_pRenderTargetView;
  ID3D11DepthStencilView* m_DepthStencilView;
  D3D_DRIVER_TYPE m_DriverType;
  D3D_FEATURE_LEVEL m_FeatureLevel;
  D3D_PRIMITIVE_TOPOLOGY m_currentPrimitiveTopology;
  D3D11_VIEWPORT m_mainViewport;
  float m_aspectRatio;

  ID3D11RasterizerState *m_d3dRasterStateDefault;
  ID3D11RasterizerState *m_d3dRasterStateSolCullBack;
  ID3D11RasterizerState *m_d3dRasterStateSolCullFront;
  ID3D11RasterizerState *m_d3dRasterStateWireframe;
  ID3D11RasterizerState *m_d3dRasterStateImgui;

  //Sampler states for textures
  ID3D11SamplerState* m_pDiffSampleState, *m_pSpecSampleState, *m_pNormalSampleState;


public:

  //extra scenarios for lighting...
  float spinAngle;
  bool useApplyAll;

  XMFLOAT4    m_CommonPosition;
  XMFLOAT4    m_CommonDirection;
  XMFLOAT4    m_CommonIa;
  XMFLOAT4    m_CommonId;
  XMFLOAT4    m_CommonIs;

  float       m_CommonSpotAngleInner;
  float       m_CommonSpotAngleOuter;
  float       m_CommonSpotFallOff;
  float       m_CommonConstantAttenuation;
  float       m_CommonLinearAttenuation;
  float       m_CommonQuadraticAttenuation;

  int         m_CommonlightType;
  int         m_CommonEnabled;

  //end extra scenarios for lighting...

  //Constant Buffers (per object / per light)
  ConstantBuffer m_constBuffer;  ID3D11Buffer *m_pConstantBuffer;
  LightProperties m_lightConstBuffer;  ID3D11Buffer *m_pLightConstantBuffer;
  bool m_drawLightSpheres, m_spinLights, m_sineWavLights, m_spinCamera;
    float m_spinningSpeed;
    float m_spinRadius;
    float m_spinCameraRadius;
    float m_spinCameraSpeed;
    float m_sineHeightScalar;
  MaterialProperties  m_materialConstBuffer; ID3D11Buffer *m_pMaterialConstantBuffer;

  RasterStates m_currentRasterState, testRasterState;
  bool m_drawFaceNormals, m_drawVertexNormals;

  //////////////////////////////////////////////////////////////////////////
  //AntTweakBar
  TwBar *m_modelPropertiesBar;
  TwBar *m_lightPropertiesBar;
  TwBar *m_extraLightPropertiesBar;
  TwBar *m_graphicsSettingsBar;

private:
  //Camera...duh
  Camera m_camera;
  float m_cameraSpeed;
  float m_cameraSpinAngle;
  float camYaw, camPitch;

  ImGuiImpl* m_imGuiImpl;
  //////////////////////////////////////////////////////////////////////////
};