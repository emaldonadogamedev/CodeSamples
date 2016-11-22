#pragma once

#include "IRendererComp.h"

class Sprite : public IRendererComponent
{
public:
  Sprite(IEntity* owner);
  virtual ~Sprite()override;

  //inherited methods
  virtual void Awake() override;
  virtual bool Initialize() override;
  
  virtual void FixedUpdate() override;
  virtual void Update(float dt) override;
  virtual void LateUpdate() override;
  
  virtual void Render(float dt, ID3D11Device* device) override;
  
  virtual void Release() override;

  MeshData texture;
};