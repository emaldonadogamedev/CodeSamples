#include "DynamicCubeMap.h"
#include "IEntity.h"
#include "MeshRenderer.h"
#include "DXUtil.h"

DynamicCubeMap::DynamicCubeMap()
{
  for (int i = 0; i < 6; ++i)
    m_dynCubeMapCameras[i].m_Up = s_defaultUps[i];

}

DynamicCubeMap::~DynamicCubeMap()
{

}

void DynamicCubeMap::Initialize(ID3D11Device* device)
{
  D3D11_TEXTURE2D_DESC texDesc;
  texDesc.Width = s_CubeMapTextureSize;
  texDesc.Height = s_CubeMapTextureSize;
  texDesc.MipLevels = 0;
  texDesc.ArraySize = 6;
  texDesc.SampleDesc.Count = 1;
  texDesc.SampleDesc.Quality = 0;
  texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  texDesc.Usage = D3D11_USAGE_DEFAULT;
  texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
  texDesc.CPUAccessFlags = 0;
  texDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS | D3D11_RESOURCE_MISC_TEXTURECUBE;

  ID3D11Texture2D* cubeTex = 0;
  HR(device->CreateTexture2D(&texDesc, 0, &cubeTex));

  D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
  rtvDesc.Format = texDesc.Format;
  rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
  rtvDesc.Texture2DArray.ArraySize = 1;
  rtvDesc.Texture2DArray.MipSlice = 0;

  for (int i = 0; i < 6; ++i)
  {
    rtvDesc.Texture2DArray.FirstArraySlice = i;
    HR(device->CreateRenderTargetView(cubeTex, &rtvDesc, &m_renderTargetViews[i]));
  }

  D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
  srvDesc.Format = texDesc.Format;
  srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
  srvDesc.TextureCube.MostDetailedMip = 0;
  srvDesc.TextureCube.MipLevels = -1;

  HR(device->CreateShaderResourceView(cubeTex, &srvDesc, &m_cubeTextureSRV));

  Memory::SafeRelease(cubeTex);

  D3D11_TEXTURE2D_DESC depthTexDesc;
  depthTexDesc.Width = s_CubeMapTextureSize;
  depthTexDesc.Height = s_CubeMapTextureSize;
  depthTexDesc.MipLevels = 1;
  depthTexDesc.ArraySize = 1;
  depthTexDesc.SampleDesc.Count = 1;
  depthTexDesc.SampleDesc.Quality = 0;
  depthTexDesc.Format = DXGI_FORMAT_D32_FLOAT; //only one float because depth test, not color
  depthTexDesc.Usage = D3D11_USAGE_DEFAULT;
  depthTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
  depthTexDesc.CPUAccessFlags = 0;
  depthTexDesc.MiscFlags = 0;

  ID3D11Texture2D* depthTex = 0;
  HR(device->CreateTexture2D(&depthTexDesc, 0, &depthTex));

  // Create the depth stencil view for the entire cube
  D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
  dsvDesc.Format = depthTexDesc.Format;
  dsvDesc.Flags = 0;
  dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
  dsvDesc.Texture2D.MipSlice = 0;
  HR(device->CreateDepthStencilView(depthTex, &dsvDesc, &m_dynamicCubeMapDSV));
  Memory::SafeRelease(depthTex);

  //set the viewport to the size of the texture... this is so coool!!
  mCubeMapViewport.TopLeftX = 0.0f;
  mCubeMapViewport.TopLeftY = 0.0f;
  mCubeMapViewport.Width = (float)s_CubeMapTextureSize;
  mCubeMapViewport.Height = (float)s_CubeMapTextureSize;
  mCubeMapViewport.MinDepth = 0.0f;
  mCubeMapViewport.MaxDepth = 1.0f;
}

void DynamicCubeMap::RenderObjectsToTextures(ID3D11Device* device, std::vector<IRendererComponent*>* objs, MeshRenderer* skip,
  ConstantBuffer& cBuffer, ID3D11Buffer * cBufferPtr)
{
  ID3D11RenderTargetView* renderTargets[1];

  //get the device context!
  ID3D11DeviceContext* context;
  device->GetImmediateContext(&context);
  
  //get the current viewport, depth stencil, etc...
  D3D11_VIEWPORT currentViewPort = { 0 };
  UINT currentVPNum = 1;
  context->RSGetViewports(&currentVPNum, &currentViewPort);
  
  ID3D11RenderTargetView* currentRTV;
  ID3D11DepthStencilView* currentDSV;
  context->OMGetRenderTargets(1, &currentRTV, &currentDSV);

  //set the dynamic cubemap viewport(the textures essentially)
  context->RSSetViewports(1, &mCubeMapViewport);

  //Update the cameras's targets, skip is the object using the dynamic cube map
  IEntity* temp = (IEntity*)skip->m_Owner;
  Transform* t = reinterpret_cast<Transform*>(temp->FindComponentByType(CT_TRANSFORM));
  UpdateCameraTargets(t->m_Position.x, t->m_Position.y, t->m_Position.z);

  //update the camera matrices
  UpdateCameraMatrices();

  for (int i = 0; i < 6; ++i)
  {
    // Clear cube map face and depth buffer.
    context->ClearRenderTargetView(m_renderTargetViews[i], reinterpret_cast<const float*>(&Colors::Silver));
    context->ClearDepthStencilView(m_dynamicCubeMapDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    // Bind cube map face as render target.
    renderTargets[0] = m_renderTargetViews[i];
    context->OMSetRenderTargets(1, renderTargets, m_dynamicCubeMapDSV);

    for (int j = 0; j < objs->size(); ++j)
    {
      //skkip the object that's getting the refelction..
      if (skip != (*(objs))[j])
      {
        //Update m_constantBuffer
        IEntity* temp = (IEntity*)(*(objs))[j]->m_Owner;
        Transform* t = reinterpret_cast<Transform*>(temp->FindComponentByType(CT_TRANSFORM));

        //cBuffer.mView = m_dynCubeMapCameras[i].m_View;
        //cBuffer.mProjection = m_dynCubeMapCameras[i].m_Projection;
        //
        ////Update constant per object buffer
        //context->UpdateSubresource(cBufferPtr, 0, nullptr,
        //  &cBuffer, 0, 0);

        //render object on to render target!
        (*(objs))[j]->Render(0, device);
      }
    }
  }

  // Restore old viewport and render targets.
  context->RSSetViewports(currentVPNum, &currentViewPort);
  renderTargets[0] = currentRTV;
  context->OMSetRenderTargets(1, renderTargets, currentDSV);

  context->PSSetShaderResources(4, 1, &m_cubeTextureSRV);
}

void DynamicCubeMap::Release()
{
  for (int i = 0; i < 6; ++i)
  {
    Memory::SafeRelease(m_renderTargetViews[i]);
    Memory::SafeRelease(m_cubeTextureSRV);
    Memory::SafeRelease(m_dynamicCubeMapDSV);
  }
}

void DynamicCubeMap::UpdateCameraTargets(float x, float y, float z)
{
  for (int i = 0; i < 6; ++i)
    UpdateCameraHelper(m_dynCubeMapCameras[i].m_Eye, x, y, z);

  //Camera X+
  UpdateCameraHelper(m_dynCubeMapCameras[0].m_At, x + 1.0f, y, z);

  //Camera X-
  UpdateCameraHelper(m_dynCubeMapCameras[1].m_At, x - 1.0f, y, z);

  //Camera Y+
  UpdateCameraHelper(m_dynCubeMapCameras[2].m_At, x, y + 1.0f, z);

  //Camera Y-
  UpdateCameraHelper(m_dynCubeMapCameras[3].m_At, x, y - 1.0f, z);

  //Camera Z+
  UpdateCameraHelper(m_dynCubeMapCameras[4].m_At, x, y, z + 1.0f);

  //Camera Z-
  UpdateCameraHelper(m_dynCubeMapCameras[5].m_At, x, y, z - 1.0f);
}

void DynamicCubeMap::UpdateCameraHelper(XMVECTOR& vec, float x, float y, float z)
{
  vec.m128_f32[0] = x;
  vec.m128_f32[1] = y;
  vec.m128_f32[2] = z;
}

void DynamicCubeMap::UpdateCameraMatrices()
{
  for (int i = 0; i < 6; ++i)
  {
    m_dynCubeMapCameras[i].m_View = 
      XMMatrixLookAtLH(m_dynCubeMapCameras[i].m_Eye, 
      m_dynCubeMapCameras[i].m_At, m_dynCubeMapCameras[i].m_Up);
    
    //Note: the aspect ratio is 1.0f because w = h, also fov is 90 deg
    m_dynCubeMapCameras[i].m_Projection = 
      XMMatrixPerspectiveFovLH(XM_PIDIV2, 1.0f, 0.01f, 1200.0f);
  }
}

const XMFLOAT3 DynamicCubeMap::s_cameraUps[6] = {
  XMFLOAT3(0.0f, 1.0f, 0.0f),  // +X
  XMFLOAT3(0.0f, 1.0f, 0.0f),  // -X
  XMFLOAT3(0.0f, 0.0f, -1.0f), // +Y
  XMFLOAT3(0.0f, 0.0f, +1.0f), // -Y
  XMFLOAT3(0.0f, 1.0f, 0.0f),	 // +Z
  XMFLOAT3(0.0f, 1.0f, 0.0f)	 // -Z
};

XMVECTOR DynamicCubeMap::s_defaultUps[6] = {
  {0.0f, 1.0f, 0.0f, 0 },  // +X
  {0.0f, 1.0f, 0.0f, 0 },  // -X};
  {0.0f, 0.0f, -1.0f, 0},  // +Y
  {0.0f, 0.0f, +1.0f, 0},  // -Y
  {0.0f, 1.0f, 0.0f, 0 },	 // +Z
  {0.0f, 1.0f, 0.0f, 0 }	 // -Z
};