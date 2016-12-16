#include "../ShaderIncludes.h"
#include "../TextureShaderIncludes.h"
#include "../LightShaderIncludes.h"

float DoAttenuation(Light light, int lightIndex, float d)
{
  if(light.LightType == DIRECTIONAL_LIGHT)
    return 1.0f;
  
  return min(
    1.0f / 
    (
       Lights[lightIndex].ConstantAttenuation + 
      (Lights[lightIndex].LinearAttenuation * d) + 
      (Lights[lightIndex].QuadraticAttenuation * d * d)
    ), 
  1.0f);
}

///INCOMPLETE!
float DoSpotlightEffect(Light light, float3 L)
{ 
  if(light.LightType != SPOT_LIGHT || light.SpotFallOff <= 0.0f)
    return 1.0f;
  
  //D - the	unit	vector	from	the	light	source	to	the	vertex
  const float3 D = normalize(light.Direction.xyz);
  const float LdotD = dot(L,D);
  
  if(LdotD < cos(light.SpotAngleOuter))
  {
    return 0;
  }
  
  else if(LdotD > cos(light.SpotAngleInner))
  {
    return 1.0f;
  }
  
  const float spotMinAngle = cos(light.SpotAngleInner);
  const float spotMaxAngle = cos(light.SpotAngleOuter);
  const float cosAlpha = LdotD;//dot( D, L);

  return pow( (cosAlpha - spotMaxAngle) / (spotMinAngle - spotMaxAngle), light.SpotFallOff);
}

float calculateFogS(float d)
{
  //200.0f is the far plane
  //0.01f is the near plane
  
  return (1200.0f - d)/(1200.0f - 0.01f);
}

float4 CalculateLighting(float4 vertexPos, float3 vertexNormal, float3 vertexTangent, float3 vertexBiTangent, float2 UVCoords)
{  
  const float3 viewVec = normalize(EyePosition - vertexPos).xyz;
  float3 lightVec;
  
  float4 totalLightResult = (float4)0;
  float4 tempLightResult = (float4)0;
  float currentAttenuation;
  float lightToVertexDistance;
  
  float3 bumpNormal;
  float3 bumpLightDir;
  
  for (int i = 0; i < MAX_LIGHTS; ++i)
  {    
    if (Lights[i].Enabled) 
    {
      //get the light vector
      //If it's a directional light then apply the same direction to all vertices
      if(Lights[i].LightType == DIRECTIONAL_LIGHT)
        lightVec = Lights[i].Position.xyz - ((m_maxCoords - m_minCoords)/2.0f).xyz;  
      
      //Otherwise use the vertex specific L vector...
      else
        lightVec = (Lights[i].Position - vertexPos).xyz;
      
      //calculate the distance of vertex to light
      lightToVertexDistance = length(lightVec);
      
      //normalize the light vector
      lightVec /= lightToVertexDistance;
      
      //update the attenuation value
      currentAttenuation = DoAttenuation(Lights[i], i, lightToVertexDistance);
      
      //ambient component
      tempLightResult = (Lights[i].m_Ia * Material.Ka);
      
      //diffuse + specular component
      float4 diffuse;
      float specPower;
      
      //Calculate the new normal if we're using normal mapping
      if(useNormalMap)
      {       
        //Expand the range of the normal value from (0, +1) to (-1, +1).
        float4 bumpColor = normalTexture.Sample(textureSampler, UVCoords);
        bumpColor = (bumpColor * 2.0f) - 1.0f;
        //bumpColor.z /= 1.0f;
        
        // Calculate the normal from the data in the bump map.
        bumpNormal = (bumpColor.x * vertexTangent) + (bumpColor.y * vertexBiTangent) + (bumpColor.z * vertexNormal);
      
        // Normalize the resulting bump normal.
        bumpNormal = normalize(bumpNormal);
        
        vertexNormal = bumpNormal;
      }
      
      if(Material.UseTexture)
      {
        diffuse = DoDiffuse( diffTexture.Sample(textureSampler, UVCoords), lightVec, vertexNormal );
        specPower = clamp( (int)(specTexture.Sample(textureSampler, UVCoords).x * 255.0f), 50.0f, 255.0f);
      }
      
      else
      {
        diffuse = DoDiffuse(Lights[i].m_Id, lightVec, vertexNormal);
        specPower = Material.SpecularPower;
      }

      
      tempLightResult += 
      ( 
        currentAttenuation * DoSpotlightEffect(Lights[i], lightVec) * 
        ( diffuse + DoSpecular(Lights[i].m_Is, viewVec, lightVec, vertexNormal, specPower) )
      );
      
      //add it to the overall result
      totalLightResult += (tempLightResult);
    }
  }
  
  return totalLightResult;
}

//--------------------------------------------------------------------------------------
// PS_PhongLighting - Return the lit color
//--------------------------------------------------------------------------------------
float4 Pixel_Shader(PS_INPUT input) : SV_Target
{
  input.UVCoords = claculateUV_Coordinates(input.modelSpacePos.xyz, textureType, m_minCoords.xyz, m_maxCoords.xyz);
  
  if(renderNormalMap)
    return normalTexture.Sample(textureSampler, input.UVCoords);
  
  else if(renderTangents)
    return float4(input.Tangent, 1.0f);
  
  else if(renderBiTangents)
    return float4(input.biTangent, 1.0f);
  
  //Add all of the I_totals on to 1 variable
  const float4 lit = CalculateLighting(input.worldSpacePos, normalize(input.Norm), normalize(input.Tangent), normalize(input.biTangent), input.UVCoords );

  const float S = calculateFogS( length( (input.worldSpacePos - EyePosition).xyz ) );
  
  const float4 I_local = Material.Emissive + (GlobalAmbient * Material.Ka) + lit;
  
  float4 I_final = (I_local * S) + ((1.0f - S) * IFog );
  
  
  if(Material.UseRefraction)
  {
    float3 incident = normalize(input.worldSpacePos - EyePosition).xyz;
    float3 refractVec = refract(incident, input.Norm, 1.0f/2.43f);
    return skyBoxTextures.Sample(samAnisotropic, refractVec);
  }
  
  else if (Material.UseReflection)
  {
    float3 incident = ( normalize(EyePosition - input.worldSpacePos)).xyz;
    float3 reflectionVec = reflect(-incident, input.Norm);
    float4 reflectionColor = (float4)1.0f;
    
    if(Material.UseStaticReflection)
      reflectionColor = skyBoxTextures.Sample(samAnisotropic, reflectionVec);
    else
      reflectionColor = dynCubeMapTextures.Sample(samAnisotropic, reflectionVec);
    
    I_final *=  reflectionColor;
  }
  
  return I_final;
}
