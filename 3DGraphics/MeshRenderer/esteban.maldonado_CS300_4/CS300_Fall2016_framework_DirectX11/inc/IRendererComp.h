#pragma once

#include "IComponent.h"
#include "VertexTypes.h"

//Forward Declarations
class VertexShader;
class  PixelShader;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct PhongMaterial;

class IRendererComponent : public IComponent
{
public:
  IRendererComponent(const ComponentType type, IEntity* owner, const bool active = true)
    :IComponent(type, owner, active), m_vertexShader(nullptr), m_pixelShader(nullptr), 
      m_phongMaterial(nullptr)
  {
  }

  virtual ~IRendererComponent()override{};
  virtual void Render(float dt, ID3D11Device* device = nullptr) = 0;
  virtual void RenderDebugVertexNormals(float dt, ID3D11Device* device = nullptr) = 0;
  virtual void RenderDebugFaceNormals(float dt, ID3D11Device* device = nullptr) = 0;

  //Inherited methods
  virtual void Awake() override = 0;
  virtual bool Initialize() override = 0;
  virtual void FixedUpdate() override = 0;
  virtual void Update(float dt) override = 0;
  virtual void LateUpdate() override = 0;
  virtual void Release() override = 0;

  VertexShader* m_vertexShader;
  PixelShader* m_pixelShader;
  PhongMaterial* m_phongMaterial;

};

typedef IRendererComponent* RenderComp;
typedef std::vector<RenderComp> RendComp_Vec;
typedef RendComp_Vec::iterator RendComp_Vec_Iter;
typedef RendComp_Vec::const_iterator RendComp_Vec_Const_Iter;

