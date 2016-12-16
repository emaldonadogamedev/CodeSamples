///////////////////////////////////////////////////////////////////////////////////
// CONST LIGHT VALUES
//Maximum amount of lights
#define MAX_LIGHTS 8

// Light types.
#define DIRECTIONAL_LIGHT 0
#define POINT_LIGHT 1
#define SPOT_LIGHT 2

///////////////////////////////////////////////////////////////////////////////////
// LIGHT STRUCTURES
struct Light
{
  float4      Position;               // 16 bytes
  //----------------------------------- (16 byte boundary)
  float4      Direction;              // 16 bytes
  //----------------------------------- (16 byte boundary)
  float4      m_Ia;                  // 16 bytes
  float4      m_Id;                  // 16 bytes
  float4      m_Is;                  // 16 bytes
  //----------------------------------- (16 byte boundary)
  float       SpotAngleInner;              // 4 bytes
  float       SpotAngleOuter;       // 4 bytes
  float       SpotFallOff;
  float       ConstantAttenuation;    // 4 bytes
  
  float       LinearAttenuation;      // 4 bytes
  float       QuadraticAttenuation;   // 4 bytes
  //----------------------------------- (16 byte boundary)
  int         LightType;              // 4 bytes
  bool        Enabled;                // 4 bytes
  //----------------------------------- (16 byte boundary)
};  // Total:                           // 80 bytes (5 * 16 byte boundary)


///////////////////////////////////////////////////////////////////////////////////
// LIGHTS CONST BUFFER
cbuffer perLightProperties : register(b2)
{
  float4 EyePosition;                 // 16 bytes
  //----------------------------------- (16 byte boundary)
  float4 GlobalAmbient;               // 16 bytes
  //----------------------------------- (16 byte boundary)
  float4 IFog;
  Light Lights[MAX_LIGHTS];           // 80 * 8 = 640 bytes
};  // Total:                           // 672 bytes (42 * 16 byte boundary)


///////////////////////////////////////////////////////////////////////////////////
// MATERIAL STRUCTURE AND CONST BUFFER
struct PhongMaterial
{
  float4  Emissive;      // 16 bytes
  //----------------------------------- (16 byte boundary)
  float  Ka;             // 4 bytes
  //------------------------------------(16 byte boundary)
  float  Diffuse;        // 4 bytes
  //----------------------------------- (16 byte boundary)
  float  Specular;       // 4 bytes
  //----------------------------------- (16 byte boundary)
  float  SpecularPower;  // 4 bytes
  
  int    UseTexture;     // 4 bytes
  int    UseNormalMap;
  int    UseBlinn;
  int    UseReflection;
  
  float4 ReflectionColor;
  
  int    UseStaticReflection;
  int    UseRefraction;
  int    padding[2];
  //----------------------------------- (16 byte boundary)
}; 

cbuffer materialPerObject : register(b3)
{
  PhongMaterial Material;
};


///////////////////////////////////////////////////////////////////////////////////
// LIGHTING FUNCTIONS

//DoDiffuse - Returns the diffuse component of the phong lighting model
// Id - the light's diffuse color
// L - light vector
// N - normal of the vertex
float4 DoDiffuse(float4 Id, float3 L, float3 N)
{
  return Id * Material.Diffuse * max( dot(N, L), 0 );
}

//DoDiffuse - Returns the diffuse component of the phong lighting model
// Is - The light's specular color
// V - View vector
// L - light vector
// N - normal of the vertex
float4 DoSpecular(float4 Is, float3 V, float3 L, float3 N, float specPower)
{  
  if (Material.UseBlinn)
  {
    // calculate half vector H
    const float3 H = normalize(L + V);
    const float NdotH = max( dot(N, H), 0);
    
    if(NdotH == 0)
    return float4(0, 0,0,0);
    
    //return blinn-phong specular
    return Is * Material.Specular * pow(  NdotH, specPower);
  }
  
  // Phong lighting.
  const float3 R = normalize( (2.0f * dot(N, L) * N) - L );
  
  const float RdotV = max( dot(R, V), 0);
  
  if(RdotV == 0)
    return float4(0, 0,0,0);
  
  //return phong specular
  return Is * Material.Specular * pow(  RdotV, specPower);
}