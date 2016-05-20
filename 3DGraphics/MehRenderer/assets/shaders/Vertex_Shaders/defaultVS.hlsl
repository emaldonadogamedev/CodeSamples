//#include "../Helper.h"

//UV projector Functions
#define PF_Planar 0
#define PF_Cylindrical 1
#define PF_Spherical   2

#define PI 3.14150f

//--------------------------------------------------------------------------------------
// Constant Buffer Variabless
//--------------------------------------------------------------------------------------
cbuffer ConstantBuffer : register(b0)
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

  int padding;
}

//--------------------------------------------------------------------------------------
struct VS_INPUT
{
  float4 Pos : POSITION;
  float3 Norm : NORMAL;
  
  float2 UV : UV;
  float2 UV_Cylindrical : UV_Cylindrical;
  float2 UV_Spherical : UV_Spherical;
  
  float3 Tangent : TANGENT;
  float3 Tangent_Cylindrical : TANGENT_Cylindrical;
  float3 Tangent_Spherical : TANGENT_Spherical;
  
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
  float4 origPos : ORIG_POSITION;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT Vertex_Shader(VS_INPUT input)
{
  PS_INPUT output = (PS_INPUT)0;
  
  if(textureType == 0)
  {
    output.UVCoords = input.UV;
    output.Tangent = input.Tangent;
    output.biTangent = input.biTangent;
  }
  
  else if(textureType == 1)
  {
    output.UVCoords = input.UV_Cylindrical;
    output.Tangent = input.Tangent_Cylindrical;
    output.biTangent = input.biTangent_Cylindrical;
  }
  
  else if(textureType == 2)
  {
    output.UVCoords = input.UV_Spherical;
    output.Tangent = input.Tangent_Spherical;
    output.biTangent = input.biTangent_Spherical;
  }
  
  output.Pos = mul(input.Pos, World);
  output.origPos = output.Pos; //keep the original point at world space
  
  output.Pos = mul(output.Pos, View);
  output.Pos = mul(output.Pos, Projection);
  output.Norm = normalize(mul(float4(input.Norm, 0), World).xyz);
  
  output.Tangent = ( mul(output.Tangent, (float3x3)World) );
  output.biTangent = ( mul(output.biTangent, (float3x3)World) );

  //Passing the default color for the light orbs
  output.DefaultColor = input.Color;
  
  return output;
}