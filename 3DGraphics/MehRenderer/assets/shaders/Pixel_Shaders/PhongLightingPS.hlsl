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

Texture2D diffTexture : register(t0);
Texture2D specTexture : register(t1);
Texture2D normalTexture : register(t2);

SamplerState textureSampler : register(s0);

///////////////////////////////////////////////////////////////////////////////////
// VERTEX AND PIXEL STRUCTURE
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
///////////////////////////////////////////////////////////////////////////////////
// CONST LIGHT VALUES
//Maximum amount of lights
#define MAX_LIGHTS 8

// Light types.
#define DIRECTIONAL_LIGHT 0
#define POINT_LIGHT 1
#define SPOT_LIGHT 2

///////////////////////////////////////////////////////////////////////////////////
// LIGHT STRUCTURE
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
cbuffer perLightProperties : register(b1)
{
  float4 EyePosition;                 // 16 bytes
  //----------------------------------- (16 byte boundary)
  float4 GlobalAmbient;               // 16 bytes
  //----------------------------------- (16 byte boundary)
  float4 IFog;
  Light Lights[MAX_LIGHTS];           // 80 * 8 = 640 bytes
};  // Total:                           // 672 bytes (42 * 16 byte boundary)

struct PhongMaterial
{
  float4  Emissive;       // 16 bytes
  //----------------------------------- (16 byte boundary)
  float4  Ka;        // 16 bytes
  //------------------------------------(16 byte boundary)
  float4  Diffuse;        // 16 bytes
  //----------------------------------- (16 byte boundary)
  float4  Specular;       // 16 bytes
  //----------------------------------- (16 byte boundary)
  float   SpecularPower;  // 4 bytes
  bool    UseTexture;     // 4 bytes
  bool    UseNormalMap;
  bool    UseBlinn;
  //----------------------------------- (16 byte boundary)
};  // Total:               // 80 bytes ( 5 * 16 )

///////////////////////////////////////////////////////////////////////////////////
// MATERIAL CONST BUFFER
cbuffer materialPerObject : register(b2)
{
  PhongMaterial Material;
}; 

float4 DoDiffuse(Light light, float3 L, float3 N)
{
  float lightIntensity = 1.0f;
  if (Material.UseNormalMap)
  {
    // Calculate the amount of light on this pixel based on the bump map normal value.
    lightIntensity = saturate(dot(N, -L));
  }

  float NdotL = max(0, dot(N, L));
  return saturate( (light.m_Id * Material.Diffuse * NdotL) * lightIntensity );
}

float4 DoSpecular(Light light, float3 V, float3 L, float3 N, float2 specUV)
{
  // Phong lighting.
  float3 R = normalize(reflect(-L, N));
  float RdotV = dot(R, V);

  // Blinn-Phong lighting
  float3 H = normalize(L + V);
  float NdotH = dot(N, H);
  
  float specPower = Material.SpecularPower;
  if (Material.UseTexture)
  {
    specPower = specTexture.Sample(textureSampler, specUV).x; 
  }
  
  if (Material.UseBlinn)
  {
    //return blinn specular
    return light.m_Is * Material.Specular * max(pow(NdotH, specPower), 0);
  }
  
  //return phong specular
  return light.m_Is * Material.Specular * max(pow(RdotV, specPower), 0);
}

float DoAttenuation(Light light, float d)
{
  return min(1.0f / (Lights[0].ConstantAttenuation + Lights[0].LinearAttenuation * d + Lights[0].QuadraticAttenuation * d * d), 1.0f);
}

struct LightingResult
{
  float4 Ambient;
  float4 Diffuse;
  float4 Specular;
};

LightingResult DoPointLight(Light light, float3 V, float4 P, float3 N, float2 specUV)
{
  LightingResult result;

  float3 L = -(light.Position - P).xyz;
  float distance = length(L);
  L = L / distance;

  float attenuation = DoAttenuation(light, distance);

  result.Ambient = light.m_Ia * Material.Ka;
  result.Diffuse = DoDiffuse(light, L, N) * attenuation;
  result.Specular = DoSpecular(light, V, L, N, specUV) * attenuation;

  return result;
}

LightingResult DoDirectionalLight(Light light, float3 V, float4 P, float3 N, float2 specUV)
{
  LightingResult result;
  
  float3 LDirection = normalize(P.xyz - light.Position.xyz);
 
  
  result.Ambient = light.m_Ia * Material.Ka;
  result.Diffuse = DoDiffuse(light, LDirection, N);
  result.Specular = DoSpecular(light, V, LDirection, N, specUV);
  
  return result;
}

float DoSpotCone(Light light, float3 L)
{
  float spotMinAngle = cos(light.SpotAngleInner);
  float spotMaxAngle = cos(light.SpotAngleOuter);
  float cosAngle = max(dot(light.Direction.xyz, -L), 0);

  //return pow(smoothstep(spotMinAngle, spotMaxAngle, cosAngle), light.SpotFallOff);
  return pow( (spotMinAngle - cosAngle) / (spotMinAngle - spotMaxAngle), light.SpotFallOff);
}

float calculateFogS(float d)
{
  //return smoothstep(0.1f, 200.0f, d);
  return (200.0f - d)/(200.0f - 0.1f);
}

LightingResult DoSpotLight(Light light, float3 V, float4 P, float3 N, float2 specUV)
{
  LightingResult result;

  float3 L = (light.Position - P).xyz;
  float distance = length(L);
  
  L = L / distance;

  float attenuation = DoAttenuation(light, distance);
  float spotIntensity = DoSpotCone(light, -L);

  result.Ambient = light.m_Ia * Material.Ka * attenuation;
  result.Diffuse = DoDiffuse(light, L, N) * attenuation * spotIntensity;
  result.Specular = DoSpecular(light, V, L, N, specUV) * attenuation * spotIntensity;

  return result;
}

LightingResult ComputeLighting(float4 P, float3 N, float2 specUV, float3 Tangent, float3 biTangent)
{
  float3 viewVec = normalize(P - EyePosition).xyz;

  LightingResult totalResult = (LightingResult)0;

  for (int i = 0; i < MAX_LIGHTS; ++i)
  {
    LightingResult result = (LightingResult)0;

    if (!Lights[i].Enabled) 
      continue;

    switch (Lights[i].LightType)
    {
      case DIRECTIONAL_LIGHT:
      {
        result = DoDirectionalLight(Lights[i], viewVec, P, N, specUV);        
      }
      break;
      
      case POINT_LIGHT:
      {
        result = DoPointLight(Lights[i], viewVec, P, N, specUV);
      }
      break;
      
      case SPOT_LIGHT:
      {
        result = DoSpotLight(Lights[i], viewVec, P, N, specUV);
      }
      break;
    }
    
    totalResult.Ambient += result.Ambient;
    totalResult.Diffuse += result.Diffuse;
    totalResult.Specular += result.Specular;
  }

  totalResult.Ambient = saturate(totalResult.Ambient);
  totalResult.Diffuse = saturate(totalResult.Diffuse);
  totalResult.Specular = saturate(totalResult.Specular);
  
  return totalResult;
}

//--------------------------------------------------------------------------------------
// PSSolid - render a solid color
//--------------------------------------------------------------------------------------
float4 Pixel_Shader(PS_INPUT input) : SV_Target
{     
  if (Material.UseNormalMap)
  {
    float4 bumpMap;

    bumpMap = normalTexture.Sample(textureSampler, input.UVCoords);

    // Expand the range of the normal value from (0, +1) to (-1, +1).
    bumpMap = (bumpMap * 2.0f) - 1.0f;

    // Calculate the normal from the data in the bump map.  
    input.Norm = -normalize( (bumpMap.x * input.Tangent) + (bumpMap.y * input.biTangent) + (bumpMap.z * input.Norm) );
    //input.Norm /= 2.0f;
  }

  LightingResult lit = ComputeLighting(input.origPos, normalize(input.Norm), input.UVCoords, input.Tangent, input.biTangent);

  float V =  length( (EyePosition - input.origPos).xyz);
  float width, height, S = calculateFogS(V);

  float4 emissive = Material.Emissive;
    float4 ambient = Material.Ka * GlobalAmbient + lit.Ambient;
    float4 diffuse = lit.Diffuse;
    float4 specular = lit.Specular;


    
    float4 texColor = { 1, 1, 1, 1 };
    //Determine whether we get diffuse from material or texture
     diffTexture.GetDimensions(width, height);
     if (width > 0 && height > 0 && Material.UseTexture)
       texColor = diffTexture.Sample(textureSampler, input.UVCoords);// * lit.Diffuse;
     // else
       // diffuse = Material.Diffuse * lit.Diffuse;
    //End getting diffuse
    
    if (Material.UseTexture)
    {
      texColor = diffTexture.Sample(textureSampler, input.UVCoords);
      //return texColor;
    }
    
    
    float4 finalColor = (emissive + ambient + diffuse + specular) * texColor;
    
    finalColor = (finalColor * S) + (IFog * (1.0f-S));

  return finalColor;
}
