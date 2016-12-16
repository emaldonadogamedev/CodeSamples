#include "../ShaderIncludes.h"
#include "../TextureShaderIncludes.h"

RasterizerState NoCull
{
    CullMode = None;
};

DepthStencilState LessEqualDSS
{
	// Make sure the depth function is LESS_EQUAL and not just LESS.  
	// Otherwise, the normalized depth values at z = 1 (NDC) will 
	// fail the depth test if the depth buffer was cleared to 1.
    DepthFunc = LESS_EQUAL;
};

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 Pixel_Shader(PS_INPUT input) : SV_Target
{
  //SetRasterizerState(NoCull);
  //SetDepthStencilState(LessEqualDSS, 0);
  
  return skyBoxTextures.Sample(textureSampler, input.modelSpacePos.xyz);
}