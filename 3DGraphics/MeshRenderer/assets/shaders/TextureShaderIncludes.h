//UV projector Function macros
#define PF_Planar 0
#define PF_Cylindrical 1
#define PF_Spherical   2

Texture2D diffTexture : register(t0);
Texture2D specTexture : register(t1);
Texture2D normalTexture : register(t2);

SamplerState textureSampler : register(s0);

float2 calculateUV_Planar(float3 entity3D, float3 min_coords, float3 max_coords)
{
  
  float2 uvResult = {
    (entity3D.x - min_coords.x) / (max_coords.x - min_coords.x),
    (entity3D.y - min_coords.y) / (max_coords.y - min_coords.y)
  };
  
  return uvResult;
}

////////////////////////////////////////////////////////////////////////////////////////
//UV helper functions
float2 calculateUV_Cylindrical(float3 entity3D, float3 min_coords, float3 max_coords)
{
  float2 uvResult = (float2)0;
  
  float3 transMatVector = {
    (max_coords.x + min_coords.x) / 2.0f,
    min_coords.y,
    //(max_coords.y + min_coords.y) / 2.0f,
    (max_coords.z + min_coords.z) / 2.0f,
  };

  float3 scaleMatVector = {
    2.0f / (max_coords.x - min_coords.x) ,
    1.0f / (max_coords.y - min_coords.y) ,
    2.0f / (max_coords.z - min_coords.z) 
  };
  
  float4x4 scaleMatrix = GetScalingMatrix(scaleMatVector.x, scaleMatVector.y, scaleMatVector.z);
  float4x4 translateMatrix = GetTranslationMatrix(transMatVector.x, transMatVector.y, transMatVector.z);
  
  float4 newVertexPos = mul( float4(entity3D, 1.0f), mul( scaleMatrix, translateMatrix) );
  
  float theta = atan2(newVertexPos.z, newVertexPos.x);
  
  while (theta < 0)
    theta += (TwoPI);

  while (theta > TwoPI)
    theta -= (TwoPI);

  uvResult.x = theta / (TwoPI);
  uvResult.y = newVertexPos.y;
  
  return uvResult;
}

float2 calculateUV_Spherical(float3 entity3D, float3 min_coords, float3 max_coords)
{
  float2 uvResult = (float2)0;
  
  float3 transMatVector = {
    (max_coords.x + min_coords.x) / 2.0f,
    (max_coords.y + min_coords.y) / 2.0f,
    //min_coords.z
    (max_coords.z + min_coords.z) / 2.0f
  };

  float3 scaleMatVector = {
    1.0f / (max_coords.x - min_coords.x) ,
    1.0f / (max_coords.y - min_coords.y) ,
    1.0f / (max_coords.z - min_coords.z) 
  };
  
  float4x4 scaleMatrix = GetScalingMatrix(scaleMatVector.x, scaleMatVector.y, scaleMatVector.z);
  float4x4 translateMatrix = GetTranslationMatrix(transMatVector.x, transMatVector.y, transMatVector.z);
  
  float4 newVertexPos = mul( float4(entity3D, 1.0f), mul( scaleMatrix, translateMatrix) );
  
  float theta = atan2(newVertexPos.y, newVertexPos.x);
  
  while (theta < 0)
    theta += (TwoPI);

  while (theta > TwoPI)
    theta -= (TwoPI);

  uvResult.x = theta / (TwoPI);
  
  float r = sqrt( (newVertexPos.x * newVertexPos.x) + (newVertexPos.y * newVertexPos.y) + (newVertexPos.z * newVertexPos.z) );
  
  float phi = r == 0 ? 0 : acos(newVertexPos.z/ r);

  while (phi < 0)
    phi += PI;

  while (phi > PI)
    phi -= PI;
  
  uvResult.y = phi / PI;
  
  return uvResult;
}
// END - UV helper functions
////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////
// Main UV function
float2 claculateUV_Coordinates(float3 entity3D, int projFuncType, float3 min_coords, float3 max_coords)
{
  if(projFuncType == PF_Planar)
  {
    return calculateUV_Planar(entity3D, min_coords, max_coords);
  }
  else if(projFuncType == PF_Cylindrical)
  {
    return calculateUV_Cylindrical(entity3D, min_coords, max_coords);
  }
  
  //else - PF_Spherical
  return calculateUV_Spherical(entity3D, min_coords, max_coords);
}
// END - Main UV function
////////////////////////////////////////////////////////////////////////////////////////

