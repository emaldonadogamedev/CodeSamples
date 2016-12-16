#pragma once

#include<vector>
#include <string>
#include<DirectXMath.h>

enum ObjectType
{
  OT_NONE = -1,

  //Game Object
  OT_GAME_OBJECT,

  //Shaders
  OT_VERTEX_SHADER,
  OT_COMPUTE_SHADER,
  OT_PIXEL_SHADER,

  //Components
  OT_TEXTURE,
  OT_STATIC_MESH,

  //Buffers
  OT_VERTEX_BUFFER,
  OT_INDEX_BUFFER,
  OT_CONSTANT_BUFFER,

  OT_RENDER_TARGET,
  OT_BACK_BUFFER,

  OT_COUNT
};

enum Format
{
  FORMAT_UCHAR4 = 28,       // <-- DXGI_FORMAT_R8G8B8A8_UNORM,
  FORMAT_UCHAR4_BGRA = 87,  // <-- DXGI_FORMAT_B8G8R8A8_UNORM,
  FORMAT_UNSIGNED1 = 42,    // <-- DXGI_FORMAT_R32_UINT,
  FORMAT_UNSIGNED2 = 17,    // <-- DXGI_FORMAT_R32G32_UINT,
  FORMAT_UNSIGNED3 = 7,     // <-- DXGI_FORMAT_R32G32B32_UINT,
  FORMAT_UNSIGNED4 = 3,     // <-- DXGI_FORMAT_R32G32B32A32_UINT,
  FORMAT_INT1 = 43,         // <-- DXGI_FORMAT_R32_SINT,
  FORMAT_INT2 = 18,         // <-- DXGI_FORMAT_R32G32_SINT,
  FORMAT_INT3 = 8,          // <-- DXGI_FORMAT_R32G32B32_SINT,
  FORMAT_INT4 = 4,          // <-- DXGI_FORMAT_R32G32B32A32_SINT,
  FORMAT_FLOAT1 = 41,       // <-- DXGI_FORMAT_R32_FLOAT,
  FORMAT_FLOAT2 = 16,       // <-- DXGI_FORMAT_R32G32_FLOAT,
  FORMAT_FLOAT3 = 6,        // <-- DXGI_FORMAT_R32G32B32_FLOAT,
  FORMAT_FLOAT4 = 2,        // <-- DXGI_FORMAT_R32G32B32A32_FLOAT
};

enum BlendMode
{
  BLEND_MODE_NONE = -1,
  BLEND_MODE_ALPHA,
  BLEND_MODE_ADDITIVE,
};

enum RenderTargetMode
{
  RTM_NORMAL,
  RTM_DOWNSAMPLED,
  RTM_FIXED
};

struct Dimension
{
  Dimension(unsigned w = 0, unsigned h = 0) : width(w), height(h) {};

  unsigned width;
  unsigned height;
};

struct InputElement
{
  InputElement(const std::string& sem, Format form, bool inst = false)
    : semantic(sem), format(form), instance(inst) {}

  std::string semantic;
  Format format;
  bool instance;
};

typedef std::vector<InputElement> InputLayout;

struct Camera
{
  DirectX::XMVECTOR m_Eye;
  DirectX::XMVECTOR m_At;
  DirectX::XMVECTOR m_Up;
  DirectX::XMMATRIX m_View;
  DirectX::XMMATRIX m_Projection;
};

struct ConstantBuffer
{
  DirectX::XMMATRIX mWorld;
  DirectX::XMMATRIX mView;
  DirectX::XMMATRIX mProjection;

  DirectX::XMFLOAT4 m_minCoords;
  DirectX::XMFLOAT4 m_maxCoords;
  DirectX::XMFLOAT4 vOutputColor;

  int useTexture;
  int textureType;
  int useNormalMap;
  int renderNormalMap = 0;

  int renderTangents = 0;
  int renderBiTangents = 0;
  int padding[2];
};