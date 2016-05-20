/*******************************************************************************
 * All content © 2015 DigiPen (USA) Corporation, all rights reserved.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 *
 * File: particleSystemVS.hlsl
 * Author: Esteban Maldonado
 * Class: GAM300/GAM302/GAM352/GAM375/GAM400
 ******************************************************************************/
#include "ShaderIncludes.h"

#define MAX_PARTICLE_COUNT 200

//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
  float3 position     : POSITION;
  float3 normal       : NORMAL;
  float3 tangent      : TANGENT;
  float3 bitangent    : BITANGENT;
  float2 uv           : TEXTURE;
  float4 color		    : COLOR;
};

struct PixelInputType
{
  float4 position : SV_POSITION;
  float2 uv : TEXTURE;
  float4 color : COLOR;
};

struct Particle_HLSL
{
  float3 pos;
  float scaleX;
  
  float3 rot;
  float scaleY;
  
  float4 color;
};

/////////////
// GLOBALS //
/////////////

cbuffer Matrix3D : register(b0)
{
	float4x4 view;
  float4x4 viewInverse;
	float4x4 projection;
  float4x4 projectionInverse;
  float4x4 previousViewProjection;
};

cbuffer ParticleHLSL_CBuffer : register(b4)
{
  Particle_HLSL particles[MAX_PARTICLE_COUNT];
  
  int particle_count;
  float padding[3];
};

cbuffer Constants : register(b2)
{
	float3 EyePosition;
	float  isSkydome;
	float4 emissiveColor;
	float offset;
}

struct VertexInfo
{
  float2 position;
  float2 UV;
};

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType main(in uint id:SV_VertexID)
{
  PixelInputType output = (PixelInputType)0;
  
  //retrieve both the particle and vertex index
  int particleIndex = id / 6;
  int vertexInQuad = id % 6;
  
  //Adjust the x,y values of the position(View Space) according to the specific vertex
  float3 position = (float3)0;
  
  VertexInfo vInfoArr[6] = {
    {-0.5f, 0.5, 0, 0},
    { 0.5f, 0.5, 1, 0},
    { 0.5f,-0.5, 1, 1},
    {-0.5f, 0.5, 0, 0},
    { 0.5f,-0.5, 1, 1},
    {-0.5f,-0.5, 0, 1}
  };
  
  position.xy = vInfoArr[vertexInQuad].position;
  output.uv = vInfoArr[vertexInQuad].UV;
  
  //scale the position
  position.x *= particles[ particleIndex ].scaleX;
  position.y *= particles[ particleIndex ].scaleY;
  
  //Multiply the position by the Z rotation matrix, this helps in keeping the particles billboarded
  position = mul(position, generateRotation_Z_Matrix(particles[ particleIndex ].rot.z));
  
  //Multiply the position by the inverse view matrix and subtract the camera position in order
  // to move it into world space
  position = mul(position,  (float3x3)viewInverse)  + particles[ particleIndex ].pos;
  

  //prepare the pixel, pass the World Space position and take it back to projection space
  output.position = float4(position, 1.0);
  output.position = mul(output.position, transpose(view) );
  output.position = mul(output.position, projection );
  
  //set the color of the specific particle
  output.color =  particles[particleIndex].color;

  return output;
}