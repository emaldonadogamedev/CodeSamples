#include "CubeMap.h"
#include "TextureManager.h"
#include "MeshGenerator.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ShaderManager.h"
#include "DXUtil.h"

CubeMap::CubeMap(std::string folder): m_individualFolder(folder)
{
  m_cubeMapTextures.resize(cubeMapTextureCount);
}

CubeMap::~CubeMap()
{
}

bool CubeMap::Initialize(ID3D11Device* device)
{
  LoadTextures(device);
  s_vertexBuffer = MeshGenerator::getObjVertexBuffer(SKY_BOX);
  s_indexBuffer = MeshGenerator::getObjIndexBuffer(SKY_BOX);

  s_vertexShader = (VertexShader*)ShaderManager::getShader("cubeMapVS");
  s_pixelShader = (PixelShader*)ShaderManager::getShader("cubeMapPS");

  D3D11_DEPTH_STENCIL_DESC dsDesc;
  ZeroMemory(&dsDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

  // Depth test parameters
  dsDesc.DepthEnable = true;
  dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
  dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

  // Stencil test parameters
  dsDesc.StencilEnable = true;
  dsDesc.StencilReadMask = 0xFF;
  dsDesc.StencilWriteMask = 0xFF;

  // Stencil operations if pixel is front-facing
  dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
  dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
  dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_ZERO;
  dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

  // Stencil operations if pixel is back-facing
  dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_ZERO;
  dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
  dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
  dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

  // Create depth stencil state
  device->CreateDepthStencilState(&dsDesc, &pDSState);

  return true;
}

void CubeMap::LoadTextures(ID3D11Device* device)
{

  for (int i = 0; i < 6; ++i)
  {
    //TextureManager::addTexture(cubeMapFolder + m_individualFolder + s_fileNames[i], false, &m_cubeMapTextures[i]);

    m_cubeMapTextures[i] = new Texture(cubeMapFolder + m_individualFolder + s_fileNames[i],false);
    m_cubeMapTextures[i]->loadTextureNoShdrResView();
  }

  D3D11_TEXTURE2D_DESC texDesc;
  texDesc.Width = m_cubeMapTextures[0]->getTextureWidth();
  texDesc.Height = m_cubeMapTextures[0]->getTextureHeight();
  texDesc.MipLevels = 1;
  texDesc.ArraySize = 6;
  texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  texDesc.CPUAccessFlags = 0;
  texDesc.SampleDesc.Count = 1;
  texDesc.SampleDesc.Quality = 0;
  texDesc.Usage = D3D11_USAGE_DEFAULT;
  texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
  texDesc.CPUAccessFlags = 0;
  texDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

  D3D11_SHADER_RESOURCE_VIEW_DESC SMViewDesc;
  SMViewDesc.Format = texDesc.Format;
  SMViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
  SMViewDesc.TextureCube.MipLevels = texDesc.MipLevels;
  SMViewDesc.TextureCube.MostDetailedMip = 0;

  D3D11_SUBRESOURCE_DATA pData[6];

  for (int cubeMapFaceInd = 0; cubeMapFaceInd < 6; cubeMapFaceInd++)
  {
    pData[cubeMapFaceInd].pSysMem = m_cubeMapTextures[cubeMapFaceInd]->getTextureDataPtr();
    pData[cubeMapFaceInd].SysMemPitch = m_cubeMapTextures[cubeMapFaceInd]->getTextureWidth() * 4;
    pData[cubeMapFaceInd].SysMemSlicePitch = 0;
  }

  HR( device->CreateTexture2D(&texDesc, &pData[0], &m_pCubeTexture) );
  HR( device->CreateShaderResourceView(m_pCubeTexture, &SMViewDesc, &m_pShaderResourceView) );
}

void CubeMap::Update(float dt)
{
}

void CubeMap::Render(float dt, ID3D11Device* device, ID3D11RasterizerState * rs)
{
  //Attach the buffers
  if (s_vertexBuffer != MeshGenerator::m_currBindedVertexBuffer)
  {
    s_vertexBuffer->attachBuffer();
    MeshGenerator::m_currBindedVertexBuffer = s_vertexBuffer;
  }
  if (s_indexBuffer != MeshGenerator::m_currBindedIndexBuffer)
  {
    s_indexBuffer->attachBuffer();
    MeshGenerator::m_currBindedIndexBuffer = s_indexBuffer;
  }

  //Set the shaders
  if (s_vertexShader != ShaderManager::m_currBindedVertexShader)
  {
    s_vertexShader->bindShader();
    ShaderManager::m_currBindedVertexShader = s_vertexShader;
  }
  if (s_pixelShader != ShaderManager::m_currBindedPixelShader)
  {
    s_pixelShader->bindShader();
    ShaderManager::m_currBindedPixelShader = s_pixelShader;
  }

  //get context
  ID3D11DeviceContext* context;
  device->GetImmediateContext(&context);

  //get a copy of the current rasterizer state
  ID3D11RasterizerState* currentRS = nullptr;
  context->RSGetState(&currentRS);

  context->RSSetState(rs);

  //apply the textures
  context->PSSetShaderResources(3, 1, &m_pShaderResourceView);

  //set the depth stencil state
  ID3D11DepthStencilState* s;
  UINT num;
  context->OMGetDepthStencilState(&s, &num);
  context->OMSetDepthStencilState(pDSState, 1);

  //draw the cube map
  context->DrawIndexed(36, 0, 0);

  //return the raster state back to normal
  context->RSSetState(currentRS);
  context->OMSetDepthStencilState(s, num);
}

void CubeMap::Release()
{
  Memory::SafeRelease(m_pShaderResourceView);
  Memory::SafeRelease(m_pCubeTexture);

  for (int i = 0; i < 6; ++i)
  {
    Memory::SafeDelete(m_cubeMapTextures[i]);
  }
}

VertexShader* CubeMap::s_vertexShader = nullptr;
PixelShader* CubeMap::s_pixelShader = nullptr;

VertexBuffer* CubeMap::s_vertexBuffer = nullptr;
IndexBuffer* CubeMap::s_indexBuffer = nullptr;

const int CubeMap::cubeMapTextureCount = 6;

const std::string CubeMap::cubeMapFolder = "CubeMaps\\";

const std::string CubeMap::s_fileNames[6] = {
  "left.tga",
  "right.tga",
  "up.tga",
  "down.tga",
  "front.tga",
  "back.tga"
};
