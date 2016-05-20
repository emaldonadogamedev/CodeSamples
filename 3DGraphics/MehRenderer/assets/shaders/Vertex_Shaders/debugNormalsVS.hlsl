//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer ConstantBuffer : register(b0)
{
  matrix World;
  matrix View;
  matrix Projection;
  float4 vOutputColor;
  bool useTexture;
}

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
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT Vertex_Shader(VS_INPUT input)
{
  PS_INPUT output = (PS_INPUT)0;
  output.Pos = mul(input.Pos, World);
  output.Pos = mul(output.Pos, View);
  output.Pos = mul(output.Pos, Projection);
  //output.Norm = mul(float4(input.Norm, 1), World).xyz;

  return output;
}