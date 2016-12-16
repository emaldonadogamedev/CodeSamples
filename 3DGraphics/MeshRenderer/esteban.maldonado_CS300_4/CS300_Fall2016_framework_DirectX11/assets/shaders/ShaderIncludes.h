#define H_FLIP       0x01
#define V_FLIP       0x02
#define SCREEN_SPACE 0x04
#define EPSILON      0.0001
#define PI 3.14150f
#define TwoPI 6.283185307f

//--------------------------------------------------------------------------------------
// Per Object - Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer ConstantBuffer : register(b1)
{
  matrix World;
  matrix View;
  matrix Projection;
  
  float4 m_minCoords;
  float4 m_maxCoords;
  
  float4 vOutputColor;
  
  int useTexture;
  int textureType;
  int useNormalMap;
  int renderNormalMap;
  int renderTangents;
  int renderBiTangents;
  
  int padding[2];
}

///////////////////////////////////////////////////////////////////////////////////
// VERTEX STRUCTURE
struct VS_INPUT
{
  float3 Pos : POSITION;
  float3 Norm : NORMAL;
  
  //UV coords
  float2 UV : UV;
  float2 UV_Cylindrical : UV_Cylindrical;
  float2 UV_Spherical: UV_Spherical;
  
  //tangents
  float3 Tangent : TANGENT;
  float3 Tangent_Cylindrical : TANGENT_Cylindrical;
  float3 Tangent_Spherical : TANGENT_Spherical;
  
  //bi tangents
  float3 biTangent : BI_TANGENT;
  float3 biTangent_Cylindrical : BI_TANGENT_Cylindrical;
  float3 biTangent_Spherical : BI_TANGENT_Spherical;
  
  float4 Color : COLOR;
};

struct PS_INPUT
{
  float4 Pos : SV_POSITION;
  float3 Norm : NORMAL;
  float3 Tangent : TANGENT;
  float2 UVCoords : UV;
  float4 DefaultColor : COLOR;
  float3 biTangent : BI_TANGENT;

  //extra
  float4 modelSpacePos : MOD_SPACE_POS;
  float4 worldSpacePos : ORIG_POSITION;
};

float4x4 GetScalingMatrix(float x, float y, float z)
{
  float4x4 result = (float4x4)0;
  
  result[0][0] = x;
  result[1][1] = y;
  result[2][2] = z;
  result[3][3] = 1.0f;
  
  return result;
}

float4x4 GetTranslationMatrix(float x, float y, float z)
{
  float4x4 result = (float4x4)0;
  
  result[0][3] = x;
  result[1][3] = y;
  result[2][3] = z;
  
  result[0][0] = result[1][1] = result[2][2] = result[3][3] = 1.0f;
  
  return result;
}