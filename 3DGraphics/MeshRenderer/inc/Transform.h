#pragma once
#include "IComponent.h"
#include <DirectXMath.h>

using DirectX::XMFLOAT2;
using DirectX::XMFLOAT3;
using DirectX::XMFLOAT4;
using DirectX::XMVECTOR;
using DirectX::XMMATRIX;

class Transform : public IComponent
{

public:
  Transform(IEntity* owner = NULL);
  virtual ~Transform()override;

  virtual bool Initialize() override;
  virtual void Update(float dt) override;
  virtual void Release() override;
  virtual void Awake() override;
  virtual void FixedUpdate() override;
  virtual void LateUpdate() override;

  XMFLOAT3 m_Centroid;
  XMMATRIX m_Centroid_matrix;
  XMMATRIX m_CentroidNeg_matrix;

  XMFLOAT3 m_Position;
  XMMATRIX m_Translation_matrix;

  XMFLOAT3 m_Rotation;
  XMMATRIX m_Rotation_matrix;

  XMFLOAT3 m_Scale;
  XMMATRIX m_Scale_matrix;

  XMMATRIX m_World_matrix;

  //Some weird matrices
  XMFLOAT3 m_Direction;
  XMMATRIX m_Local_matrix;
  XMMATRIX m_MView_matrix;
  XMMATRIX m_MViewProj_matrix;

private:
  void UpdateMatrices();
};

