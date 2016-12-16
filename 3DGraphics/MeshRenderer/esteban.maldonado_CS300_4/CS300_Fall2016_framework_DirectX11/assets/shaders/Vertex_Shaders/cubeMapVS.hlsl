#include "../ShaderIncludes.h"

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
//PS_INPUT Vertex_Shader(VS_INPUT input, in uint id:SV_VertexID)
PS_INPUT Vertex_Shader(VS_INPUT input)
{
  PS_INPUT output = (PS_INPUT)0;
  
  output.UVCoords = input.UV;
  
  //keep the original position at model space
  output.modelSpacePos = float4(input.Pos, 1.0f);
  
  output.Pos = mul( float4(input.Pos, 1.0f), World);
  output.worldSpacePos = output.Pos; //keep the original point at world space
  
  //move the position of the vertex to the view space
  output.Pos = mul(output.Pos, View);
  
  //move the position of the vertex to the projection space
  //keep z equal to w so that z/w is 1, and the cube map is on the far plane
  output.Pos = mul(output.Pos, Projection).xyww;
  
  //Passing the default color for the light orbs
  output.DefaultColor =  input.Color;
  
  return output;
}