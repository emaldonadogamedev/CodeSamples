#include "GraphicsManager.h"
#include "TextureManager.h"
#include "MeshGenerator.h"
#include "ShaderManager.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "IEntity.h"
#include "AntTweakBar/include/AntTweakBar.h"

#include "MeshRenderer.h"
#include "InputManager.h"

#include <windows.h>
#include <VersionHelpers.h>

#include "ImGuiImpl.h"


using namespace Memory;

GraphicsManager* GraphicsManager::m_Instance = nullptr;

GraphicsManager::GraphicsManager() :ISystem(ST_Graphics, "Graphics")
{
  //Default clear color
  m_clearScreenColor = DirectX::Colors::Blue;

  m_hAppInstance = NULL;
  m_hAppWnd = NULL;
  m_ClientWidth = DEFAULT_WINDOW_WIDTH;
  m_ClientHeight = DEFAULT_WINDOW_HEIGHT;
  m_AppTitle = "Meh Renderer - CS300";
  m_MainWndCaption = "D3D11 Application";
  m_WndStyle = WS_OVERLAPPEDWINDOW;

  //d3d attributes default value
  m_pDevice = NULL;
  m_pImmediateContext = NULL;
  m_pRenderTargetView = NULL;
  m_pSwapChain = NULL;
  m_DepthStencilBuffer = NULL;
  m_DepthStencilView = NULL;

  //Const Buffers
  m_pConstantBuffer = NULL;
  m_pLightConstantBuffer = NULL;
  m_pMaterialConstantBuffer = NULL;

  m_currentPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

  m_currentRasterState = testRasterState = RS_SOLID_DEFAULT;
  m_drawFaceNormals = false;
  m_drawVertexNormals = false;
  m_drawLightSpheres = false;
  m_spinLights = m_spinCamera = m_sineWavLights = false;
  m_spinningSpeed = m_sineHeightScalar = 0;

  m_pDiffSampleState = nullptr;
  m_pSpecSampleState = nullptr;
  m_pNormalSampleState = nullptr;

  m_Instance = this;

  m_constBuffer.textureType = 1;

  m_cameraSpeed = camYaw = camPitch = 0;
  m_spinRadius = 2.5f;
  m_spinCameraRadius = 8.0f;
  spinAngle = m_cameraSpinAngle = 0;
  m_spinCameraSpeed = .3f;

  //Extra common light properties
  m_CommonIa = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
  m_CommonId = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
  m_CommonIs = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

  m_CommonSpotAngleInner = DirectX::XM_PIDIV4 / 3.0f;
  m_CommonSpotAngleOuter = DirectX::XM_PIDIV4;
  m_CommonSpotFallOff = 1.0f;
  m_CommonConstantAttenuation = 1.0f;
  m_CommonLinearAttenuation = 0.1f;
  m_CommonQuadraticAttenuation = 0;

  m_CommonlightType = PointLight;
  m_CommonEnabled = false;
}

GraphicsManager::~GraphicsManager()
{

}

GraphicsManager* GraphicsManager::GetInstance()
{
  if (!m_Instance)
  {
    void* temp = _aligned_malloc( sizeof(GraphicsManager), 16);

    m_Instance = new (temp) GraphicsManager();
  }

  return m_Instance;
}

GraphicsManager** GraphicsManager::GetInstanceRef()
{
  if (!m_Instance)
  {
    void* temp = _aligned_malloc(sizeof(GraphicsManager), 16);
    m_Instance = new (temp) GraphicsManager();
  }

  return &m_Instance;
}

ID3D11Device* GraphicsManager::GetDevice() const
{
  return m_pDevice;
}

ID3D11DeviceContext* GraphicsManager::GetContext() const
{
  return m_pImmediateContext;
}

IDXGISwapChain* GraphicsManager::GetSwapChain() const
{
  return m_pSwapChain;
}

ID3D11RenderTargetView** GraphicsManager::GetRenderTargetView()
{
  return &m_pRenderTargetView;
}

D3D_PRIMITIVE_TOPOLOGY& GraphicsManager::GetPrimitiveTopology()
{
  return m_currentPrimitiveTopology;
}

HWND GraphicsManager::GetWindowHandle()
{
  return m_hAppWnd;
}

void GraphicsManager::AddComponent(const RenderComp comp)
{
  m_Components.push_back(comp);
}

void GraphicsManager::RemoveComponent(const RenderComp comp)
{
  //for (RendComp_Vec_Const_Iter it = m_Components.begin();
  //  it != m_Components.end(); it++)
  //{
  //  if (*it == comp)
  //    m_Components.erase(it);
  //}
  comp->m_IsActive = false;
}

bool GraphicsManager::InitWindow()
{
  //WNDXCLASSEX
  WNDCLASSEX wcex;
  ZeroMemory(&wcex, sizeof(WNDCLASSEX));
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = 0;
  wcex.cbSize = sizeof(WNDCLASSEX);
  wcex.style = CS_HREDRAW | CS_VREDRAW;
  wcex.hInstance = m_hAppInstance;
  wcex.lpfnWndProc = m_MessageProc;
  wcex.hIcon = LoadIcon(NULL, IDI_HAND);
  wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
  wcex.lpszMenuName = 0;
  wcex.lpszClassName = "DXAPPWNDCLASS";
  wcex.hIconSm = LoadIcon(NULL, IDI_HAND);

  if (!RegisterClassEx(&wcex))
  {
    OutputDebugString("\nFAILED TO CREATE WINDOW CLASS\n");
    return false;
  }

  RECT r = { 0, 0, m_ClientWidth, m_ClientHeight };
  AdjustWindowRect(&r, m_WndStyle, false);
  UINT width = r.right - r.left;
  UINT height = r.bottom - r.top;
  UINT x = GetSystemMetrics(SM_CXSCREEN) / 2 - width / 2;
  UINT y = GetSystemMetrics(SM_CYSCREEN) / 2 - height / 2;

  m_hAppWnd = CreateWindow("DXAPPWNDCLASS", m_AppTitle.c_str(), m_WndStyle, x, y,
    width, height, 0, 0, m_hAppInstance, 0);

  if (!m_hAppWnd)
  {
    OutputDebugString("\nFAILED TO CREATE WINDOW\n");
    return false;
  }

  ShowWindow(m_hAppWnd, SW_SHOW);

  return true;
}


bool GraphicsManager::InitDirect3D()
{
  UINT createDeviceFlags = 0;

#ifdef _DEBUG
  //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif //_DEBUG

  D3D_DRIVER_TYPE driverTypes[] = 
  {
    D3D_DRIVER_TYPE_HARDWARE, //<- video card,best choice
      D3D_DRIVER_TYPE_WARP, //<- emulated
      D3D_DRIVER_TYPE_REFERENCE
  };

  UINT numDriverTypes = ARRAYSIZE(driverTypes);

  D3D_FEATURE_LEVEL featureLevels[] = 
  {
    D3D_FEATURE_LEVEL_11_0,
      D3D_FEATURE_LEVEL_10_1,
      D3D_FEATURE_LEVEL_10_0,
      D3D_FEATURE_LEVEL_9_3
  };

  UINT numFeatureLevels = ARRAYSIZE(featureLevels);

  //Prepare double buffer description
  DXGI_SWAP_CHAIN_DESC swapDesc;
  ZeroMemory(&swapDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

  swapDesc.BufferCount = 1; // double buffered
  swapDesc.BufferDesc.Width = m_ClientWidth;
  swapDesc.BufferDesc.Height = m_ClientHeight;
  swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
  swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
  swapDesc.BufferDesc.RefreshRate.Numerator = 60;
  swapDesc.BufferDesc.RefreshRate.Denominator = 1;
  swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  swapDesc.OutputWindow = m_hAppWnd;
  swapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
  swapDesc.Windowed = true;
  swapDesc.SampleDesc.Count = 1;
  swapDesc.SampleDesc.Quality = 0;
  swapDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; //alt-enter fullscreen
  //end double buff description

  //Loop through the driver types to find the best supported one.
  HRESULT result;
  for (UINT i = 0; i < numDriverTypes; ++i)
  {
    result = D3D11CreateDeviceAndSwapChain(0, driverTypes[i], 0, createDeviceFlags,
      featureLevels, numFeatureLevels, D3D11_SDK_VERSION, &swapDesc, &m_pSwapChain,
      &m_pDevice, &m_FeatureLevel, &m_pImmediateContext);

    if (SUCCEEDED(result))
    {
      m_DriverType = driverTypes[i];

      //OH SNAP! AN ACTUAL REASON TO USE A GOTO!!!
      goto continue_Init;
    }
  }

  OutputDebugString("FAILED TO CREATE DEVICE AND SWAP CHAIN");
  HR(result);//<-- By this point it'll fail for sure
  return false;

continue_Init:

  //CREATE RENDER TARGET VIEW
  ID3D11Texture2D* m_pBackBufferTex = 0;
  HR(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&m_pBackBufferTex)));

  HR(m_pDevice->CreateRenderTargetView(m_pBackBufferTex, NULL, &m_pRenderTargetView));
  SafeRelease(m_pBackBufferTex);

  //CREATE DEPTH STENCIL BUFFER
  D3D11_TEXTURE2D_DESC depthStencilDesc;
  depthStencilDesc.Width = m_ClientWidth;
  depthStencilDesc.Height = m_ClientHeight;
  depthStencilDesc.MipLevels = 1;
  depthStencilDesc.ArraySize = 1;
  depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
  depthStencilDesc.SampleDesc.Count = 1;
  depthStencilDesc.SampleDesc.Quality = 0;
  depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
  depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
  depthStencilDesc.CPUAccessFlags = 0;
  depthStencilDesc.MiscFlags = 0;

  HR(result = m_pDevice->CreateTexture2D(&depthStencilDesc, 0, &m_DepthStencilBuffer));

  D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
  ZeroMemory(&dsvd, sizeof(dsvd));

  dsvd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
  dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
  HR(result = m_pDevice->CreateDepthStencilView(m_DepthStencilBuffer, &dsvd, &m_DepthStencilView));

  //BIND RENDER TARGET VIEW
  m_pImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_DepthStencilView);

  // Setup rasterizer states
  D3D11_RASTERIZER_DESC rasterizerDesc;
  ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

  rasterizerDesc.AntialiasedLineEnable = FALSE;
  rasterizerDesc.CullMode = D3D11_CULL_NONE;
  rasterizerDesc.DepthBias = 0;
  rasterizerDesc.DepthBiasClamp = 0.0f;
  rasterizerDesc.DepthClipEnable = TRUE;
  rasterizerDesc.FillMode = D3D11_FILL_SOLID;
  rasterizerDesc.FrontCounterClockwise = FALSE;
  rasterizerDesc.MultisampleEnable = FALSE;
  rasterizerDesc.ScissorEnable = FALSE;
  rasterizerDesc.SlopeScaledDepthBias = 0.0f;

  // Create the default rasterizer state object.
  m_pDevice->CreateRasterizerState(&rasterizerDesc, &m_d3dRasterStateDefault);

  //Create the back face cull mode
  rasterizerDesc.CullMode = D3D11_CULL_BACK;
  m_pDevice->CreateRasterizerState(&rasterizerDesc, &m_d3dRasterStateSolCullBack);

  //Create the front face cull mode
  rasterizerDesc.CullMode = D3D11_CULL_FRONT;
  m_pDevice->CreateRasterizerState(&rasterizerDesc, &m_d3dRasterStateSolCullFront);

  //Create the wireframe rasterizer state object
  rasterizerDesc.CullMode = D3D11_CULL_NONE;
  rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
  rasterizerDesc.AntialiasedLineEnable = TRUE;
  m_pDevice->CreateRasterizerState(&rasterizerDesc, &m_d3dRasterStateWireframe);

  D3D11_RASTERIZER_DESC RSDesc;
  memset(&RSDesc, 0, sizeof(D3D11_RASTERIZER_DESC));
  //create the rasterizer state for ImGui
  RSDesc.FillMode = D3D11_FILL_SOLID;
  RSDesc.CullMode = D3D11_CULL_NONE;
  RSDesc.FrontCounterClockwise = FALSE;
  RSDesc.DepthBias = 0;
  RSDesc.SlopeScaledDepthBias = 0.0f;
  RSDesc.DepthBiasClamp = 0;
  RSDesc.DepthClipEnable = TRUE;
  RSDesc.ScissorEnable = TRUE;
  RSDesc.AntialiasedLineEnable = FALSE;
  RSDesc.MultisampleEnable = swapDesc.SampleDesc.Count > 1 ? TRUE : FALSE;
  m_pDevice->CreateRasterizerState(&RSDesc, &m_d3dRasterStateImgui);

  m_pImmediateContext->RSSetState(m_d3dRasterStateDefault);

  //VIEWPORT CREATION
  m_Viewport.Width = static_cast<float>(m_ClientWidth);
  m_Viewport.Height = static_cast<float>(m_ClientHeight);
  m_Viewport.TopLeftX = m_Viewport.TopLeftY = 0;
  m_Viewport.MinDepth = 0.0f;
  m_Viewport.MaxDepth = 1.0f;

  //Bind viewport
  m_pImmediateContext->RSSetViewports(1, &m_Viewport);

#pragma region Spread_The_Word
  //////////////////////////////////////////////////////////////////////////
  // SPREAD THE WORD OF THE DEVICE AND THE CONTEXT
  //Begin Inform the buffer class of the device and device context
  VertexBuffer::m_Direct3DDevice = this->m_pDevice;
  VertexBuffer::m_Direct3DDeviceContext = this->m_pImmediateContext;

  IndexBuffer::m_Direct3DDevice = this->m_pDevice;
  IndexBuffer::m_Direct3DDeviceContext = this->m_pImmediateContext;

  Shader::m_Direct3DDevice = this->m_pDevice;
  Shader::m_Direct3DDeviceContext = this->m_pImmediateContext;
  //End Inform the buffer class of the device and device context
  //////////////////////////////////////////////////////////////////////////
#pragma endregion

  return true;
}

bool GraphicsManager::InitAntTweakBar()
{
  TwInit(TW_DIRECT3D11, m_pDevice); // for Direct3D 11
  TwWindowSize(m_ClientWidth, m_ClientHeight);
  m_modelPropertiesBar = TwNewBar("Model_Properties");
  m_graphicsSettingsBar = TwNewBar("Graphics_Properties");
  m_lightPropertiesBar = TwNewBar("Light_Properties");
  m_extraLightPropertiesBar = TwNewBar("Extra_Light_Properties");

  return true;
}

bool GraphicsManager::InitImGui()
{
  m_imGuiImpl->Initialize(m_hAppWnd, m_pDevice, m_pImmediateContext);

  return true;
}

bool GraphicsManager::InitConstantBuffer()
{
  //Prepare camera(default values)
  m_camera.m_Eye = XMVectorSet(0.0f, 0.0f, -10.0f, 0.0f);
  m_camera.m_At = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
  m_camera.m_Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
  m_camera.m_View = XMMatrixLookAtLH(m_camera.m_Eye, m_camera.m_At, m_camera.m_Up);
  m_camera.m_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, m_aspectRatio = ( (float)m_ClientWidth / (float)m_ClientHeight), 0.01f, 200.0f);
  //m_camera.m_Projection = XMMatrixOrthographicLH(m_ClientWidth, m_ClientHeight, 0.01f, 1000.0f);

  //Initial constant buffer values
  m_constBuffer.mWorld = XMMatrixIdentity(); // Set the world matrix at the origin
  m_constBuffer.mView = XMMatrixTranspose(m_camera.m_View);
  m_constBuffer.mProjection = XMMatrixTranspose(m_camera.m_Projection);
  m_constBuffer.vOutputColor = XMFLOAT4(1, 0, 1, 1);
 
  // Create the constant buffer
  D3D11_BUFFER_DESC bd;
  ZeroMemory(&bd, sizeof(bd));
  bd.Usage = D3D11_USAGE_DEFAULT;
  bd.ByteWidth = sizeof(ConstantBuffer);
  bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  bd.CPUAccessFlags = 0;
  HR(m_pDevice->CreateBuffer(&bd, nullptr, &m_pConstantBuffer));

  m_pImmediateContext->UpdateSubresource(m_pConstantBuffer, 0, nullptr, &m_constBuffer, 0, 0);

  m_pImmediateContext->VSSetConstantBuffers(1 /*register b1*/, 1, &m_pConstantBuffer);
  m_pImmediateContext->PSSetConstantBuffers(1 /*register b1*/, 1, &m_pConstantBuffer);

  return true;
}

bool GraphicsManager::InitLightConstantBuffer()
{
  //Set Default values to lights
  //... TO DO

  // Create the constant buffer
  D3D11_BUFFER_DESC bd;
  int n = 0;
  ZeroMemory(&bd, sizeof(bd));
  bd.Usage = D3D11_USAGE_DEFAULT;
  bd.ByteWidth = n = sizeof(LightProperties);
  bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  bd.CPUAccessFlags = 0;
  HR(m_pDevice->CreateBuffer(&bd, nullptr, &m_pLightConstantBuffer));

  m_pImmediateContext->UpdateSubresource(m_pLightConstantBuffer, 0, nullptr, &m_lightConstBuffer, 0, 0);
  m_pImmediateContext->PSSetConstantBuffers(2 /*register b2*/, 1, &m_pLightConstantBuffer);

  return true;
}

bool GraphicsManager::InitMaterialConstantBuffer()
{
  // Create the constant buffer
  D3D11_BUFFER_DESC bd;
  ZeroMemory(&bd, sizeof(bd));
  bd.Usage = D3D11_USAGE_DEFAULT;
  bd.ByteWidth = sizeof(PhongMaterial);
  bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  bd.CPUAccessFlags = 0;
  HR(m_pDevice->CreateBuffer(&bd, nullptr, &m_pMaterialConstantBuffer));

  m_pImmediateContext->UpdateSubresource(m_pMaterialConstantBuffer, 0, nullptr, &m_materialConstBuffer, 0, 0);
  m_pImmediateContext->PSSetConstantBuffers(3 /*register b3*/, 1, &m_pMaterialConstantBuffer);

  return true;
}

bool GraphicsManager::InitTextureSamplerStates()
{
  D3D11_SAMPLER_DESC samplerDesc;
  ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));

  HRESULT result;

  // Create a texture sampler state description.
  samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
  samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
  samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
  samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
  samplerDesc.MipLODBias = 0.0f;
  samplerDesc.MaxAnisotropy = 1;
  samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;// D3D11_COMPARISON_ALWAYS;
  samplerDesc.BorderColor[0] = 0;
  samplerDesc.BorderColor[1] = 0;
  samplerDesc.BorderColor[2] = 0;
  samplerDesc.BorderColor[3] = 0;
  samplerDesc.MinLOD = 0;
  samplerDesc.MaxLOD = 0;

  // Create the texture sampler state.
  result = m_pDevice->CreateSamplerState(&samplerDesc, &m_pDiffSampleState);
  result = m_pDevice->CreateSamplerState(&samplerDesc, &m_pSpecSampleState);
  result = m_pDevice->CreateSamplerState(&samplerDesc, &m_pNormalSampleState);

  return true;
}

void GraphicsManager::ResizeSwapChain()
{
  m_pImmediateContext->OMSetRenderTargets(0, nullptr, nullptr);

  // First release the render target and depth/stencil views.
  SafeRelease(m_pRenderTargetView);
  SafeRelease(m_DepthStencilView);
  SafeRelease(m_DepthStencilBuffer);

  TwWindowSize(m_ClientWidth, m_ClientHeight);

  // Resize the swap chain buffers.
  m_pSwapChain->ResizeBuffers(1, m_ClientWidth, m_ClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

  // Next initialize the back buffer of the swap chain and associate it to a 
  // render target view.
  ID3D11Texture2D* backBuffer;
  HR(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**) &backBuffer));

  HR(m_pDevice->CreateRenderTargetView(backBuffer, nullptr, &m_pRenderTargetView));

  //Update Projection Matrices
  m_camera.m_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, 
    m_aspectRatio = ( (float)m_ClientWidth / (float)m_ClientHeight ) , 0.01f, 200.0f);

  //Initial constant buffer values
  m_constBuffer.mView = XMMatrixTranspose(m_camera.m_View);
  m_constBuffer.mProjection = XMMatrixTranspose(m_camera.m_Projection);

  m_pImmediateContext->UpdateSubresource(m_pConstantBuffer, 0, nullptr, &m_constBuffer, 0, 0);

  m_pImmediateContext->VSSetConstantBuffers(1, 1, &m_pConstantBuffer);
  m_pImmediateContext->PSSetConstantBuffers(1, 1, &m_pConstantBuffer);

  //CREATE DEPTH STENCIL BUFFER
  SafeRelease(m_DepthStencilBuffer);
  SafeRelease(m_DepthStencilView);

  D3D11_TEXTURE2D_DESC depthStencilDesc;
  depthStencilDesc.Width = m_ClientWidth;
  depthStencilDesc.Height = m_ClientHeight;
  depthStencilDesc.MipLevels = 1;
  depthStencilDesc.ArraySize = 1;
  depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
  depthStencilDesc.SampleDesc.Count = 1;
  depthStencilDesc.SampleDesc.Quality = 0;
  depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
  depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
  depthStencilDesc.CPUAccessFlags = 0;
  depthStencilDesc.MiscFlags = 0;

  HR(m_pDevice->CreateTexture2D(&depthStencilDesc, 0, &m_DepthStencilBuffer));

  D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
  ZeroMemory(&dsvd, sizeof(dsvd));

  dsvd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
  dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
  HR(m_pDevice->CreateDepthStencilView(m_DepthStencilBuffer, &dsvd, &m_DepthStencilView));

  //BIND RENDER TARGET VIEW
  m_pImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_DepthStencilView);

  //VIEWPORT CREATION
  m_Viewport.Width = static_cast<float>(m_ClientWidth);
  m_Viewport.Height = static_cast<float>(m_ClientHeight);
  m_Viewport.TopLeftX = m_Viewport.TopLeftY = 0;
  m_Viewport.MinDepth = 0.0f;
  m_Viewport.MaxDepth = 1.0f;

  //Bind viewport
  m_pImmediateContext->RSSetViewports(1, &m_Viewport);
}

void GraphicsManager::RenderLightSpheres()
{
  for (int i = 0; i < MAX_LIGHTS; ++i)
  {
    XMFLOAT4* p(&m_lightConstBuffer.Lights[i].m_Position);
    m_constBuffer.mWorld = XMMatrixTranspose( XMMatrixTranslation(p->x, p->y, p->z));
    m_constBuffer.vOutputColor = m_lightConstBuffer.Lights[i].m_Id;

    //Update constant per object buffer
    m_pImmediateContext->UpdateSubresource(m_pConstantBuffer, 0, nullptr,
      &m_constBuffer, 0, 0);

    MeshBuffer* mB = MeshGenerator::getObjMeshBuffer(SPHERE_MESH);
    PixelShader* s = (PixelShader*)ShaderManager::getShader("LightSpherePS");

    s->bindShader();
    ShaderManager::m_currBindedPixelShader = s;

    mB->vBuffer->attachBuffer();
    mB->iBuffer->attachBuffer();
    MeshGenerator::m_currBindedVertexBuffer = mB->vBuffer;
    MeshGenerator::m_currBindedIndexBuffer = mB->iBuffer;

    m_pImmediateContext->DrawIndexed(mB->index_count, 0, 0);
  }
}

void GraphicsManager::UpdateCamera(float dt)
{
  if (m_spinCamera)
  {
    if (m_cameraSpinAngle > DirectX::XM_2PI)
      m_cameraSpinAngle = 0;

    m_cameraSpinAngle += m_spinCameraSpeed * dt;

    m_camera.m_Eye.m128_f32[0] = m_spinCameraRadius * cos(m_cameraSpinAngle);
    m_camera.m_Eye.m128_f32[2] = m_spinCameraRadius * sin(m_cameraSpinAngle);
  }

  m_cameraSpeed = 12.0f * dt;

  //update position through keyboard
  if (InputManager::GetInstance()->m_keyboard.IsKeyDown(DIK_A))
  {
    m_camera.m_Eye.m128_f32[0] -= m_cameraSpeed;
  }
  
  if (InputManager::GetInstance()->m_keyboard.IsKeyDown(DIK_D))
  {
    m_camera.m_Eye.m128_f32[0] += m_cameraSpeed;
  }

  if (InputManager::GetInstance()->m_keyboard.IsKeyDown(DIK_W))
  {
    m_camera.m_Eye.m128_f32[2] -= m_cameraSpeed;
  }

  if (InputManager::GetInstance()->m_keyboard.IsKeyDown(DIK_S))
  {
    m_camera.m_Eye.m128_f32[2] += m_cameraSpeed;
  }

  if (InputManager::GetInstance()->m_keyboard.IsKeyDown(DIK_Q))
  {
    m_camera.m_Eye.m128_f32[1] += m_cameraSpeed;
  }

  if (InputManager::GetInstance()->m_keyboard.IsKeyDown(DIK_E))
  {
    m_camera.m_Eye.m128_f32[1] -= m_cameraSpeed;
  }

  //update look at
  DIMOUSESTATE* curr, *last;

  curr = InputManager::GetInstance()->m_mouse.CurrentState();
  last = InputManager::GetInstance()->m_mouse.LastState();
  camYaw += last->lX * 0.001f;
  camPitch += curr->lY * 0.001f;

  m_camera.m_View = XMMatrixLookAtLH(m_camera.m_Eye, m_camera.m_At, m_camera.m_Up);
  m_camera.m_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, m_aspectRatio = ((float)m_ClientWidth / (float)m_ClientHeight), 0.01f, 200.0f);

  //m_constBuffer.mWorld = XMMatrixIdentity(); // Set the world matrix at the origin
  m_constBuffer.mView = XMMatrixTranspose(m_camera.m_View);
  m_constBuffer.mProjection = XMMatrixTranspose(m_camera.m_Projection);

}

void GraphicsManager::SetInstance(const HINSTANCE hInstance)
{
  m_hAppInstance = hInstance;
}

void GraphicsManager::SetMessageProc(const WNDPROC& proc)
{
  m_MessageProc = proc;
}

void GraphicsManager::SetPrimitiveTopology(int pt)
{
  m_pImmediateContext->IASetPrimitiveTopology(
    m_currentPrimitiveTopology = (D3D11_PRIMITIVE_TOPOLOGY) pt);
}

void GraphicsManager::SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY pt)
{
  m_pImmediateContext->IASetPrimitiveTopology(m_currentPrimitiveTopology = pt);
}

void GraphicsManager::SetMaterialProperties(MaterialProperties& mat)
{
  m_materialConstBuffer = mat;
  m_pImmediateContext->UpdateSubresource(m_pMaterialConstantBuffer, 0, 
    nullptr, &m_materialConstBuffer, 0, 0);
}

void GraphicsManager::UpdateLightCommonProperties(XMFLOAT4& cam_position, XMFLOAT4& fogColor /*= XMFLOAT4(1,1,1,1)*/)
{
  m_lightConstBuffer.EyePosition = cam_position;

  //setting Ifog to clear buffer color
  m_lightConstBuffer.IFog = fogColor;
}


void GraphicsManager::SetAllLightsToCommon()
{
  Light* tempLight = &m_lightConstBuffer.Lights[0];

  for (int i = 0; i < MAX_LIGHTS; ++i)
  {
    //enabled & type
    tempLight->m_Enabled = m_CommonEnabled;
    tempLight->m_lightType = m_CommonlightType;

    //spotlight properties...
    tempLight->m_SpotAngleInner = m_CommonSpotAngleInner;
    tempLight->m_SpotAngleOuter = m_CommonSpotAngleOuter;
    tempLight->m_SpotFallOff = m_CommonSpotFallOff;

    //colors
    tempLight->m_Ia = m_CommonIa;
    tempLight->m_Id = m_CommonId;
    tempLight->m_Is = m_CommonIs;

    //attenuation
    tempLight->m_ConstantAttenuation = m_CommonConstantAttenuation;
    tempLight->m_LinearAttenuation = m_CommonLinearAttenuation;
    tempLight->m_QuadraticAttenuation = m_CommonQuadraticAttenuation;

    tempLight = &m_lightConstBuffer.Lights[i + 1];
  }
}

void GraphicsManager::SetAllLightsToDifferent()
{
  Light* tempLight = &m_lightConstBuffer.Lights[0];
  int type = 0;
  for (int i = 0; i < MAX_LIGHTS; ++i)
  {
    //enabled & type
    tempLight->m_Enabled = true;
    tempLight->m_lightType = type++; 

    if (type > 2) 
      type = 0;

    //spotlight properties...
    tempLight->m_SpotAngleInner = DirectX::XM_PIDIV4 + ( MathHelper::RandF() * (DirectX::XM_PI / 6.0f) );
    tempLight->m_SpotAngleOuter = tempLight->m_SpotAngleInner * 1.2f;
    tempLight->m_SpotFallOff = MathHelper::RandF();

    //colors
    tempLight->m_Ia = MathHelper::RandF4();
    tempLight->m_Id = MathHelper::RandF4();
    tempLight->m_Is = MathHelper::RandF4();

    //attenuation
    tempLight->m_ConstantAttenuation =  MathHelper::RandF() * 5.0f;
    tempLight->m_LinearAttenuation =    MathHelper::RandF() * 5.0f;
    tempLight->m_QuadraticAttenuation = MathHelper::RandF() * 5.0f;

    tempLight = &m_lightConstBuffer.Lights[i + 1];
  }
}

void GraphicsManager::SetAllLightsToRandom()
{
  Light* tempLight = &m_lightConstBuffer.Lights[0];

  for (int i = 0; i < MAX_LIGHTS; ++i)
  {
    //enabled & type
    tempLight->m_Enabled = rand() % 1000 < 500 ? 1 : 0;
    int t = rand() % 1500;
    if (t < 500)
      tempLight->m_lightType = 0; //directional
    else if (t < 1000)
      tempLight->m_lightType = 1; //point light
    else
      tempLight->m_lightType = 2; //spot light

    //spotlight properties...
    tempLight->m_SpotAngleInner = DirectX::XM_PIDIV4 + (MathHelper::RandF() * (DirectX::XM_PI / 6.0f));
    tempLight->m_SpotAngleOuter = tempLight->m_SpotAngleInner * 1.2f;
    tempLight->m_SpotFallOff = MathHelper::RandF();

    //colors
    tempLight->m_Ia = MathHelper::RandF4();
    tempLight->m_Id = MathHelper::RandF4();
    tempLight->m_Is = MathHelper::RandF4();

    //attenuation
    tempLight->m_ConstantAttenuation = MathHelper::RandF() * 5.0f;
    tempLight->m_LinearAttenuation = MathHelper::RandF() * 5.0f;
    tempLight->m_QuadraticAttenuation = MathHelper::RandF() * 5.0f;

    tempLight = &m_lightConstBuffer.Lights[i + 1];
  }
}

void GraphicsManager::ReAddShaders()
{
  ShaderManager::reAddShader("defaultVS");
  ShaderManager::reAddShader("defaultPS");
  ShaderManager::reAddShader("PhongLightingPS");
  ShaderManager::reAddShader("LightSpherePS");
}

bool GraphicsManager::Initialize()
{
  if (!InitWindow())
    return false;

  if (!InitDirect3D())
    return false;

  if (!InitConstantBuffer() )
    return false;

  if (!InitLightConstantBuffer() || !InitMaterialConstantBuffer())
    return false;

  if (!InitAntTweakBar())
    return false;

  if(!InitTextureSamplerStates())
    return false;

  //Load Shaders
  ShaderManager::addShader(VERTEX, "defaultVS");
  ShaderManager::addShader(PIXEL, "defaultPS");
  ShaderManager::addShader(PIXEL, "PhongLightingPS");
  ShaderManager::addShader(PIXEL, "LightSpherePS");
  ShaderManager::m_defaultVertexShader = ShaderManager::getShader("defaultVS");
  ShaderManager::m_defaultPixelShader = ShaderManager::getShader("defaultPS");

  //Load basic shapes
  MeshData temp;
  MeshGenerator::CreateBox(temp, 1, 1, 1 );
  MeshGenerator::CreateQuad(temp, 1, 1);
  MeshGenerator::CreateSphere(temp, 0.3f, 60, 60); // make the default sphere

  // Set primitive topology
  m_pImmediateContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  //Set the default values for all lights - All but light 0 will be turned off
  m_lightConstBuffer.Lights[0].m_Enabled = true;
  m_lightConstBuffer.EyePosition = XMFLOAT4(m_camera.m_Eye.m128_f32);

  TextureManager::addTexture("metal_roof_spec_512x512.tga");
  TextureManager::addTexture("metal_roof_diff_512x512.tga");
  TextureManager::addTexture("metal_roof_spec_512x512.tga", true);

  for (RendComp_Vec_Const_Iter it = m_Components.begin();
    it != m_Components.end(); it++)
  {
    if ((*it)->m_IsActive)
      (*it)->Initialize();
  }

  //initialize ImGui
  if (m_imGuiImpl = new ImGuiImpl())
    m_imGuiImpl->Initialize(m_hAppWnd, m_pDevice, m_pImmediateContext);

  return m_isInitialized = true;
}

void GraphicsManager::Update(float dt)
{
  if (m_currentRasterState != testRasterState)
  {
    switch (m_currentRasterState = testRasterState)
    {
    case RS_SOLID_DEFAULT:
      m_pImmediateContext->RSSetState(m_d3dRasterStateDefault);
      break;
    case RS_WIREFRAME:
      m_pImmediateContext->RSSetState(m_d3dRasterStateWireframe);
      break;
    case RS_SOLID_CULL_BACK:
      m_pImmediateContext->RSSetState(m_d3dRasterStateSolCullBack);
      break;
    case RS_SOLID_CULL_FRONT:
      m_pImmediateContext->RSSetState(m_d3dRasterStateSolCullFront);
      break;

    default:
      m_pImmediateContext->RSSetState(m_d3dRasterStateDefault);
      break;
    }
  }

  static const float lightAngleDiff = DirectX::XM_2PI / (float)MAX_LIGHTS;

  //Spin lights (if enabled)
  if (m_spinLights && m_spinningSpeed > 0)
  {
    spinAngle += (m_spinningSpeed * dt);

    if (spinAngle > DirectX::XM_2PI)
      spinAngle = 0;

    //place all of the lights around the origin...
    for (int j = 0;
      j < MAX_LIGHTS;
      j++)
    {
      m_spinRadius = MathHelper::Max<float>(m_spinRadius, 2.5f);

      float tempAngle = spinAngle + ((j + 1) * lightAngleDiff);
      XMFLOAT4* pPos = &m_lightConstBuffer.Lights[j].m_Position;

      pPos->x = m_spinRadius * cos(tempAngle);
      pPos->z = m_spinRadius * sin(tempAngle);
    }
  }

  if (m_sineWavLights && m_sineHeightScalar > 0)
  {
    //place all of the lights around the origin...
    for (int j = 0;
      j < MAX_LIGHTS;
      j++)
    {

      float tempAngle = spinAngle + ((j + 1) * lightAngleDiff);
      XMFLOAT4* pPos = &m_lightConstBuffer.Lights[j].m_Position;
      pPos->y = m_sineHeightScalar * cos(tempAngle);

    }
  }

  UpdateCamera(dt);

  for (RendComp_Vec_Const_Iter it = m_Components.begin();
    it != m_Components.end(); it++)
  {
    if ((*it)->m_IsActive)
    {
      IEntity* temp = (*it)->m_Owner;
      Transform* t = reinterpret_cast<Transform*>(temp->FindComponentByType(CT_TRANSFORM));
      t->Update(dt);

      (*it)->Update(dt);
    }
  }

  //m_imGuiImpl->Update(dt);
}

void GraphicsManager::Render(float dt)
{
  m_pImmediateContext->ClearRenderTargetView(m_pRenderTargetView, m_clearScreenColor);
  m_pImmediateContext->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

  //Update common Light properties
  UpdateLightCommonProperties(XMFLOAT4(m_camera.m_At.m128_f32), XMFLOAT4(m_clearScreenColor.f));
  m_pImmediateContext->UpdateSubresource(m_pLightConstantBuffer, 0,
  nullptr, &m_lightConstBuffer, 0, 0);

  for (RendComp_Vec_Const_Iter it = m_Components.begin();
    it != m_Components.end(); it++)
  {
    if ((*it)->m_IsActive)
    {
      //Update m_constantBuffer
      IEntity* temp = (*it)->m_Owner;
      Transform* t = reinterpret_cast<Transform*>(temp->FindComponentByType(CT_TRANSFORM));

      //add the min and max coordinates for UV calculations
      m_constBuffer.m_minCoords = reinterpret_cast<MeshRenderer*>(*it)->m_Mesh.m_minCoords;
      m_constBuffer.m_maxCoords = reinterpret_cast<MeshRenderer*>(*it)->m_Mesh.m_maxCoords;

      m_constBuffer.mWorld = t->m_World_matrix;
      m_constBuffer.useTexture = (*it)->m_phongMaterial->UseTexture;
      m_constBuffer.useNormalMap = (*it)->m_phongMaterial->UseNormalMap;

      //Update constant per object buffer
      m_pImmediateContext->UpdateSubresource(m_pConstantBuffer, 0, nullptr,
        &m_constBuffer, 0, 0);

      //Update material object
      m_materialConstBuffer.Material = *((*it)->m_phongMaterial);
      m_pImmediateContext->UpdateSubresource(m_pMaterialConstantBuffer, 0, nullptr,
        &m_materialConstBuffer, 0, 0);

      m_pImmediateContext->PSSetSamplers(0, 1, &m_pDiffSampleState);

      //Render the object
      (*it)->Render(dt, m_pDevice);

      if (m_drawLightSpheres)
        RenderLightSpheres();

      if (m_drawFaceNormals || m_drawVertexNormals)
      {
        m_pImmediateContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
        
        if (m_drawFaceNormals)
          (*it)->RenderDebugFaceNormals(dt, m_pDevice);

        if (m_drawVertexNormals)
          (*it)->RenderDebugVertexNormals(dt, m_pDevice);
        
        m_pImmediateContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
      }
    }
  }

  TwDraw();

  HR( m_pSwapChain->Present(0, 0) );
}

void GraphicsManager::ShutDown()
{
  TwDeleteAllBars();
  TwTerminate();
  m_imGuiImpl->Cleanup();

  for (RendComp_Vec_Const_Iter it = m_Components.begin();
    it != m_Components.end(); it++)
  {
    (*it)->Release();
    delete (*it);
  }
  m_Components.clear();

  SafeRelease(m_pLightConstantBuffer);
  SafeRelease(m_pMaterialConstantBuffer);
  SafeRelease(m_pConstantBuffer);

  ShaderManager::clearShaders();
  MeshGenerator::ReleaseMeshBuffers();
  TextureManager::releaseTextures();

  SafeRelease(m_d3dRasterStateDefault);
  SafeRelease(m_d3dRasterStateSolCullBack);
  SafeRelease(m_d3dRasterStateSolCullFront);
  SafeRelease(m_d3dRasterStateWireframe);
  SafeRelease(m_d3dRasterStateImgui);

  //CLEANUP DIRECT3D
  if (m_pImmediateContext)
    m_pImmediateContext->ClearState();

  SafeRelease(m_pRenderTargetView);
  SafeRelease(m_pSwapChain);
  SafeRelease(m_pImmediateContext);
  SafeRelease(m_pDevice);
}

//WINDOW METHODS
void GraphicsManager::ResizeWindow(int width, int height)
{
  if (width <= 0)
    width = 50;
  
  if (height <= 0)
    height = 50;

  m_ClientWidth = width;
  m_ClientHeight = height;

  ResizeSwapChain();
}