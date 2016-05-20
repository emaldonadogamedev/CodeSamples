//#include "../Helper.hlsli"

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer ConstantBuffer : register(b0)
{
  matrix World;
  matrix View;
  matrix Projection;
  float4 vOutputColor;
}

/*
struct VertexTexture
{
XMFLOAT3 position;
XMFLOAT3 normal;
XMFLOAT3 tangent;
XMFLOAT2 uv;
XMFLOAT4 color;

//extra
XMFLOAT3 biTangent;
}
*/
//--------------------------------------------------------------------------------------
struct VS_INPUT
{
  float4 Pos : POSITION;
  float3 Norm : NORMAL;
  float3 Tangent : TANGENT;
  float2 UVCoords : UV;
  float4 Color : COLOR;
  float3 biTangent : BI_TANGENT;
};

struct PS_INPUT
{
  float4 Pos : SV_POSITION;
  float3 Norm : TEXCOORD0;
  float4 DefaultColor : COLOR;
};


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 Pixel_Shader(PS_INPUT input) : SV_Target
{
  return input.DefaultColor;
}