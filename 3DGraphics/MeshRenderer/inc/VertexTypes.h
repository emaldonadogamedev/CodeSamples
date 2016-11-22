#pragma once

#include <DirectXMath.h>

using DirectX::XMFLOAT2;
using DirectX::XMFLOAT3;
using DirectX::XMFLOAT4;

struct VertexWire
{ 
  XMFLOAT3 position;
  XMFLOAT4 color;

  VertexWire():position(0,0,0), color(1,1,1,1){}
  VertexWire(XMFLOAT3 pos, XMFLOAT4 col): position(pos), color(col){}
};

struct VertexNormal
{
  XMFLOAT3 position;
  XMFLOAT3 normal;
  XMFLOAT4 color;

  VertexNormal() :position(0, 0, 0), normal(0, 0, 0), color(1, 1, 1, 1){}
  VertexNormal(XMFLOAT3 pos, XMFLOAT3 norm, XMFLOAT4 col) :position(pos),
    normal(norm), color(col){}
};

struct VertexTexture
{
  XMFLOAT3 position;
  XMFLOAT3 normal;

  XMFLOAT2 uv;
  XMFLOAT2 uv_Cylindrical;
  XMFLOAT2 uv_Spherical;

  XMFLOAT3 tangent;
  XMFLOAT3 tangent_Cylindrical;
  XMFLOAT3 tangent_Spherical;

  XMFLOAT3 biTangent;
  XMFLOAT3 biTangent_Cylindrical;
  XMFLOAT3 biTangent_Spherical;

  XMFLOAT4 color;


  VertexTexture() :position(0, 0, 0), normal(0, 0, 0), tangent(0, 0, 0), biTangent(0,0,0), uv(0, 0), color(1, 1, 1, 1){}
  VertexTexture(XMFLOAT3 pos, XMFLOAT3 norm, XMFLOAT2 uv, XMFLOAT3 tangent, XMFLOAT4 col) :position(pos),
    normal(norm), uv(uv), tangent(tangent), color(col){}
  VertexTexture(
    float px, float py, float pz,
    float nx, float ny, float nz,
    float tx, float ty, float tz,
    float u, float v,
    XMFLOAT4 col = XMFLOAT4(1,1, 1,1) )
    : position(px, py, pz), normal(nx, ny, nz),
    uv(u,v),
    uv_Cylindrical(u,v),
    uv_Spherical(u,v),
    tangent(tx, ty, tz),
    tangent_Cylindrical(tx, ty, tz),
    tangent_Spherical(tx, ty, tz),
    color(col)
  {
  }
};

struct MeshFace
{
  MeshFace():normal(0,0,0){}

  unsigned int faceID;
  unsigned int v1_ID, v2_ID, v3_ID;
  XMFLOAT3 normal;
  XMFLOAT3 tangent;
  XMFLOAT3 biTangent;
};

struct MeshData
{
  std::vector<MeshFace> Faces;
  std::vector<VertexTexture> Vertices;
  std::vector<unsigned int> Indices;

  XMFLOAT4 m_minCoords;
  XMFLOAT4 m_maxCoords;
};

struct PhongMaterial
{
  PhongMaterial()
    : Emissive(0.0f, 0.0f, 0.0f, 0.0f)
    , Ka(1.0f)// , 0.25f, 0.25f, 1.0f)
    , Kd(1.0f)//, 0.25f, 0.25f, 1.0f)
    , Ks(1.0f)//, 0.25f, 0.25f, 1.0f)
    , padding(0)
    , SpecularPower(10.0f)
    , UseTexture(0)
    , UseNormalMap(0)
    , UseBlinn(0)
  {}

  XMFLOAT4   Emissive;
  //---------------------------- (16 byte boundary)
  float   Ka;
  //---------------------------- (16 byte boundary)
  float   Kd;
  //---------------------------- (16 byte boundary)
  float   Ks;
  float padding;
  //----------------------------------- (16 byte boundary)
  float SpecularPower;
  // Add some padding complete the 16 byte boundary.
  int UseTexture;
  int UseNormalMap;
  int UseBlinn;
  //----------------------------------- (16 byte boundary)
}; // Total:                                80 bytes (5 * 16)

struct MaterialProperties
{
  PhongMaterial   Material;
};

enum LightType
{
  DirectionalLight,
  PointLight,
  SpotLight
};

#define MAX_LIGHTS 8

struct Light
{
  Light()
    : m_Position(0.0f, 0.0f, 0.0f, 1.0f)
    , m_Direction(0.0f, 0.0f, 1.0f, 0.0f)
    , m_Ia(0.0f, 0.0f, 0.0f, 1.0f)
    , m_Id(0.8f, 0.8f, 0.8f, 1.0f)
    , m_Is(1.0f, 1.0f, 1.0f, 1.0f)
    , m_SpotAngleInner(DirectX::XM_PIDIV4 / 3.0f)
    , m_SpotAngleOuter(DirectX::XM_PIDIV4)
    , m_SpotFallOff(1.0f)
    , m_ConstantAttenuation(1.0f)
    , m_LinearAttenuation(0.1f)
    , m_QuadraticAttenuation(0.0f)
    , m_lightType(PointLight)
    , m_Enabled(false)
  {
  }

  XMFLOAT4    m_Position;
  XMFLOAT4    m_Direction;
  XMFLOAT4    m_Ia;
  XMFLOAT4    m_Id;
  XMFLOAT4    m_Is;

  float       m_SpotAngleInner;
  float       m_SpotAngleOuter;
  float       m_SpotFallOff;
  float       m_ConstantAttenuation;
  float       m_LinearAttenuation;
  float       m_QuadraticAttenuation;

  int   m_lightType;
  int   m_Enabled;
  // Add some padding to make this struct size a multiple of 16 bytes.
  //int         Padding;
};

struct LightProperties
{
  LightProperties():
      EyePosition(0.0f, 0.0f, 0.0f, 1.0f)
    , GlobalAmbient(0.2f, 0.2f, 0.2f, 1.0f)
    , IFog(0,0,0,1.0f)
  {}
  XMFLOAT4   EyePosition;
  //---------------------------- (16 byte boundary)
  XMFLOAT4   GlobalAmbient;
  XMFLOAT4   IFog;
  //---------------------------- (16 byte boundary)
  Light               Lights[MAX_LIGHTS]; // 80 * 8 bytes
};  // Total:                                  672 bytes (42 * 16)