#include "../ShaderIncludes.h"
#include "../TextureShaderIncludes.h"

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT Vertex_Shader(VS_INPUT input)
{
  PS_INPUT output = (PS_INPUT)0;
  
  //keep the original position at model space
  output.modelSpacePos = float4(input.Pos, 1.0f);
  
  output.Pos = mul( float4(input.Pos, 1.0f), World);
  output.worldSpacePos = output.Pos; //keep the original point at world space
  
  //move the position of the vertex to the view space
  output.Pos = mul(output.Pos, View);
  
  //move the position of the vertex to the projection space
  output.Pos = mul(output.Pos, Projection);
  
  //move the normal, tangent and bitangent vectors to the world space
  output.Norm = normalize( mul( float4(input.Norm, 0), World).xyz);
  
    //Assign the right TBN values
  if(textureType == PF_Planar)
  {
    output.Tangent = input.Tangent;
    output.biTangent = input.biTangent;
  }
  
  else if(textureType == PF_Spherical )
  {  
    output.Tangent = input.Tangent_Cylindrical;
    output.biTangent = input.biTangent_Cylindrical;
  }
  else //PF_Spherical PF_Cylindrical
  {
    output.Tangent = input.Tangent_Spherical;
    output.biTangent = input.biTangent_Spherical;
  }
  
  output.Tangent = normalize( mul( float4(output.Tangent, 0), World).xyz );
  output.biTangent = normalize( mul( float4(output.biTangent, 0), World).xyz );

  //Passing the default color for the light orbs
  output.DefaultColor =  input.Color;
  
  return output;
}