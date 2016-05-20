//--------------------------------------------------------------------------------------
// Constant Buffer Variables
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
  float3 Norm : TEXCOORD0;
  float4 DefaultColor : COLOR;
};


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 Pixel_Shader(PS_INPUT input) : SV_Target
{
  return vOutputColor;
}

float4 Pixel_Shader1(PS_INPUT input) : SV_Target
{
  return float4(0,1,1,1);//input.DefaultColor - 1.0f;
}

////////////////////////////////////////////////////////////////////////////////
// Technique
////////////////////////////////////////////////////////////////////////////////
technique10 ColorTechnique
{
  pass pass0
  {
    SetPixelShader(CompileShader(ps_4_0, Pixel_Shader1()) );
  }

  pass pass1
  {
    SetPixelShader( CompileShader(ps_4_0, Pixel_Shader1()) );
  }
}