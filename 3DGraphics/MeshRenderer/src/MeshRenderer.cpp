#include "MeshRenderer.h"
#include "MeshGenerator.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "ShaderManager.h"
#include "TextureManager.h"
#include "IEntity.h"
#include "DXUtil.h"

using namespace Memory;

MeshRenderer::MeshRenderer(IEntity* owner, const std::string meshFile) :IRendererComponent(CT_STATIC_MESH, owner),
  m_meshFile(meshFile)
{
  m_Mesh.Vertices = std::vector<VertexTexture>();
  m_Mesh.Indices = std::vector<UINT>();
  m_vertexBuffer = nullptr;
  m_indexBuffer = nullptr;
  index_count = 0;
  vertex_count = 0;

  m_vertexBufferVertexNormals = nullptr;
  m_indexBufferVertexNormals = nullptr;

  m_vertexBufferFaceNormals = nullptr;
  m_indexBufferFaceNormals = nullptr;

  //DEFAULT PEARL VALUES FOR MATERIAL
  m_phongMaterial = new PhongMaterial();
  m_phongMaterial->UseTexture = true;
  m_phongMaterial->SpecularPower = 10.0f;

  //Texture
  m_diffTexture = nullptr;
  m_specTexture = nullptr;
  m_normalTexture = nullptr;
}

MeshRenderer::~MeshRenderer()
{
  //These will be deleted on the Mesh Manager
  //SafeDelete(m_vertexBuffer);
  //SafeDelete(m_indexBuffer);

  Release();
}

void MeshRenderer::Awake()
{

}

bool MeshRenderer::Initialize()
{
  MeshBuffer& temp = MeshGenerator::CS300LoadMeshFromOBJ(m_Mesh, m_meshFile);

  m_vertexBuffer = temp.vBuffer;
  m_indexBuffer = temp.iBuffer;
  index_count = temp.index_count;
  vertex_count = temp.vertex_count;

  m_vertexBuffer->createBuffer( m_Mesh.Vertices.data(), m_Mesh.Vertices.size());
  m_indexBuffer->createBuffer(m_Mesh.Indices.data(), m_Mesh.Indices.size());

  m_vertexBufferVertexNormals = MeshGenerator::getObjVertexVNormalBuffer(m_meshFile);
  m_vertexBufferFaceNormals = MeshGenerator::getObjVertexFaceNormalBuffer(m_meshFile);

  Transform* t = &m_Owner->m_Transform;
  t->m_Centroid = temp.m_centroid;

  m_vertexShader = reinterpret_cast<VertexShader*>(ShaderManager::getShader("defaultVS"));
  m_pixelShader = reinterpret_cast<PixelShader*>(ShaderManager::getShader("PhongLightingPS"));
  
  return true;
}

void MeshRenderer::FixedUpdate()
{

}

void MeshRenderer::Update(float dt)
{
  
}

void MeshRenderer::Render(float dt, ID3D11Device* device)
{
  if (device && m_vertexBuffer && m_indexBuffer && m_vertexShader && m_pixelShader)
  {
    //get the immediate context
    ID3D11DeviceContext* context;
    device->GetImmediateContext(&context);

    //Attach the buffers
    if (m_vertexBuffer != MeshGenerator::m_currBindedVertexBuffer)
    {
      m_vertexBuffer->attachBuffer();
      MeshGenerator::m_currBindedVertexBuffer = m_vertexBuffer;
    }
    if (m_indexBuffer != MeshGenerator::m_currBindedIndexBuffer)
    {
      m_indexBuffer->attachBuffer();
      MeshGenerator::m_currBindedIndexBuffer = m_indexBuffer;
    }

    //Set the shaders
    if (m_vertexShader != ShaderManager::m_currBindedVertexShader)
    {
      m_vertexShader->bindShader();
      context->IASetInputLayout(m_vertexShader->getD3DInputLayoutPtr());
      ShaderManager::m_currBindedVertexShader = m_vertexShader;
    }
    
    if (m_pixelShader != ShaderManager::m_currBindedPixelShader)
    {
      m_pixelShader->bindShader();
      ShaderManager::m_currBindedPixelShader = m_pixelShader;
    }

    if (m_phongMaterial->UseTexture)
      BindTextures(context);

    //Draw Indexed
    context->DrawIndexed(index_count, 0, 0);
  }
}

void MeshRenderer::RenderDebugVertexNormals(float dt, ID3D11Device* device)
{
  if (device && m_vertexBufferVertexNormals)
  {
    PixelShader* temp = (PixelShader*)ShaderManager::getShader("defaultPS");
    if (!temp)
      return;
    
    MeshBuffer* rMB = MeshGenerator::getObjMeshBuffer(m_meshFile + DEBUG_VERTEX_NORMAL);
    if (!rMB)
      return;

    //get the immediate context
    ID3D11DeviceContext* context;
    device->GetImmediateContext(&context);

    temp->bindShader();
    ShaderManager::m_currBindedPixelShader = temp;

    m_vertexBufferVertexNormals->attachBuffer();
    MeshGenerator::m_currBindedVertexBuffer = m_vertexBufferVertexNormals;
    context->Draw(rMB->vertex_count, 0);
  }
}

void MeshRenderer::RenderDebugFaceNormals(float dt, ID3D11Device* device)
{
  if (device && m_vertexBufferVertexNormals)
  {
    PixelShader* temp = (PixelShader*)ShaderManager::getShader("defaultPS");
    if (!temp)
      return;

    MeshBuffer* rMB = MeshGenerator::getObjMeshBuffer(m_meshFile + DEBUG_FACE_NORMAL);
    if (!rMB)
      return;

    //get the immediate context
    ID3D11DeviceContext* context;
    device->GetImmediateContext(&context);

    //bind the pixel shader & update the manager
    temp->bindShader();
    ShaderManager::m_currBindedPixelShader = temp;
    
    //Attach the vBuffer and update the mesh manager.
    MeshGenerator::m_currBindedVertexBuffer = m_vertexBufferFaceNormals;
    m_vertexBufferFaceNormals->attachBuffer();
    context->Draw(rMB->vertex_count, 0);
  }
}

void MeshRenderer::LateUpdate()
{

}

void MeshRenderer::Release()
{
  SafeDelete(m_phongMaterial);
  //m_indexBuffer->releaseBuffer();
  //m_vertexBuffer->releaseBuffer();
}

void MeshRenderer::ChangeModel(std::string objName)
{
  MeshBuffer* temp = MeshGenerator::getObjMeshBuffer(objName);
  if (temp)
  {
    m_vertexBuffer = temp->vBuffer;
    m_indexBuffer = temp->iBuffer;
    index_count = temp->index_count;
    vertex_count = temp->vertex_count;
    m_meshFile = objName;

    m_vertexBufferFaceNormals = MeshGenerator::getObjVertexFaceNormalBuffer(objName);
    m_vertexBufferVertexNormals = MeshGenerator::getObjVertexVNormalBuffer(objName);
  }
  
  else
  {
    //If no mesh buffer found, then try and load the model
    MeshBuffer t = MeshGenerator::CS300LoadMeshFromOBJ(m_Mesh, objName);
    m_vertexBuffer = t.vBuffer;
    m_indexBuffer = t.iBuffer;
    index_count = t.index_count;
    vertex_count = t.vertex_count;

    m_vertexBufferFaceNormals = MeshGenerator::getObjVertexFaceNormalBuffer(objName);
    m_vertexBufferVertexNormals = MeshGenerator::getObjVertexVNormalBuffer(objName);

    m_meshFile = objName;
  }
}

void MeshRenderer::SetTexture(Texture* texture)
{
  if (texture)
  {
    m_diffTexture = texture;
  }
}

void MeshRenderer::SetSpecTexture(Texture* texture /*= nullptr*/)
{
  if (texture)
  {
    m_specTexture = texture;
  }
}

void MeshRenderer::SetNormalTexture(Texture* texture)
{
  if (texture)
  {
    m_normalTexture = texture;
  }
}

void MeshRenderer::BindTextures(ID3D11DeviceContext* context)
{
  if (m_diffTexture)
    context->PSSetShaderResources(0, 1, m_diffTexture->getTextureShdrResViewRef());

  if (m_specTexture)
    context->PSSetShaderResources(1, 1, m_specTexture->getTextureShdrResViewRef());

  if (m_normalTexture)
    context->PSSetShaderResources(2, 1, m_normalTexture->getTextureShdrResViewRef());
}
