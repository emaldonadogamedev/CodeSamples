#include "../ShaderIncludes.h"


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 Pixel_Shader(PS_INPUT input) : SV_Target
{
  return input.DefaultColor;
}