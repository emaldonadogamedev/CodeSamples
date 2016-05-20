#define H_FLIP       0x01
#define V_FLIP       0x02
#define SCREEN_SPACE 0x04
#define EPSILON      0.0001

inline float2x2 ComputeRotationMtx(float rads)
{
  float cosine = cos(rads);
  float sine = sin(rads);

  return float2x2 (cosine, -sine,
    sine, cosine);
}

float2 processUVCoordinates(int projFuncType, float4 position)
{
  //get the copy of the position in a bounded box form, range 0 to 1
  position.x = (position.x - m_minCoords.x) / (m_maxCoords.x - m_minCoords.x);
  position.y = (position.y - m_minCoords.y) / (m_maxCoords.y - m_minCoords.y);
  position.z = (position.z - m_minCoords.z) / (m_maxCoords.z - m_minCoords.z);
  
  float2 result = (float2)0;
  float theta = 0;
  float phi = 0;
  const float r = sqrt((position.x * position.x) + (position.y * position.y) + (position.z * position.z));

  if(projFuncType == PF_Cylindrical)
  {
    theta = atan(position.y / position.x);
    
    while(theta < 0)
      theta += (2.0 * PI);
    
    result.x = theta / (2.0 * PI);
    result.y = position.z;//(position.z - m_minCoords.z) / (m_maxCoords.z - m_minCoords.z);
  
  }
  
  else if(projFuncType == PF_Spherical)
  {
    theta = atan(position.y / position.x);
    phi = acos(position.z / r);
    while(theta < 0)
      theta += (2.0 * PI);
    
    result.x = theta / (2.0 * PI);

    while(phi < 0)
      phi += (2.0 * PI);
    
    result.y = phi / PI;
  }

  return result;
}