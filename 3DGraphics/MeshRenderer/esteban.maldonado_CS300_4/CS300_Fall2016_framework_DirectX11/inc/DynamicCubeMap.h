#pragma once

#include "IRendererComp.h"
#include "GFXTypes.h"
#include<d3d11.h>

//Forward Declarations
class VertexShader;
class  PixelShader;
class VertexBuffer;
class IndexBuffer;
class Texture;
class MeshRenderer;

////D3D forward declarations
//struct ID3D11Device;
//struct ID3D11RasterizerState;
//struct ID3D11Texture2D;
//struct ID3D11ShaderResourceView;
//struct ID3D11DepthStencilState;
//struct ID3D11RenderTargetView;


using DirectX::XMFLOAT3;
using DirectX::XMVECTOR;

class DynamicCubeMap
{
public:
  DynamicCubeMap();
  ~DynamicCubeMap();

  void Initialize(ID3D11Device* device);
  void RenderObjectsToTextures(ID3D11Device* device, std::vector<IRendererComponent*>*  objs, 
    MeshRenderer* skip, ConstantBuffer& cBuffer, ID3D11Buffer * cBufferPtr);
  void Release();

  static const XMFLOAT3 s_cameraUps[6];

private:
  void UpdateCameraTargets(float x, float y, float z);
  void UpdateCameraHelper(XMVECTOR& vec, float x, float y, float z);
  void UpdateCameraMatrices();

  Camera m_dynCubeMapCameras[6];
  ID3D11RenderTargetView* m_renderTargetViews[6];
  ID3D11ShaderResourceView* m_cubeTextureSRV = 0;
  ID3D11DepthStencilView* m_dynamicCubeMapDSV;
  D3D11_VIEWPORT mCubeMapViewport;

  static XMVECTOR s_defaultUps[6];
  static const int s_CubeMapTextureSize = 1024;
};