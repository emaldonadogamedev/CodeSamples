#pragma once

#include "IRendererComp.h"

class VertexBuffer;
class IndexBuffer;
class Texture;
struct PhongMaterial;

class MeshRenderer : public IRendererComponent
{
public:
  MeshRenderer(IEntity* owner, const std::string meshFile = "");
  ~MeshRenderer();

  virtual void Render(float dt, ID3D11Device* device) override;
  virtual void RenderDebugVertexNormals(float dt, ID3D11Device* device) override;
  virtual void RenderDebugFaceNormals(float dt, ID3D11Device* device) override;

  virtual void Awake() override;
  virtual bool Initialize() override;
  virtual void FixedUpdate() override;
  virtual void Update(float dt) override;
  virtual void LateUpdate() override;
  virtual void Release() override;

  //Setters
  virtual void ChangeModel(std::string objName);
  virtual void SetTexture(Texture* texture = nullptr);
  virtual void SetSpecTexture(Texture* texture = nullptr);
  virtual void SetNormalTexture(Texture* texture = nullptr);

  MeshData m_Mesh;
  std::string m_meshFile;

  //regular buffers
  VertexBuffer* m_vertexBuffer;
  IndexBuffer* m_indexBuffer;
  unsigned int index_count;
  unsigned int vertex_count;
  
  //vertex normals buffers
  VertexBuffer* m_vertexBufferVertexNormals;
  IndexBuffer* m_indexBufferVertexNormals;
  
  //face normals buffers
  VertexBuffer* m_vertexBufferFaceNormals;
  IndexBuffer* m_indexBufferFaceNormals;
  
  //Textures
  Texture* m_diffTexture;
  Texture* m_specTexture;
  Texture* m_normalTexture;

private:
  virtual void BindTextures(ID3D11DeviceContext* context);
};