#include "DXUtil.h"
#include "Transform.h"


Transform::Transform(IEntity* owner) : IComponent(CT_TRANSFORM, owner),
m_Centroid(0,0,0),
m_Position(0,0,0),
m_Rotation(0,0,0),
m_Scale(1,1,1)
{
}


Transform::~Transform()
{
}

bool Transform::Initialize()
{
  return true;
}

void Transform::FixedUpdate()
{
}

void Transform::Update(float dt)
{
  UpdateMatrices();
}

void Transform::LateUpdate()
{
}

void Transform::UpdateMatrices()
{
  m_Centroid_matrix = XMMatrixTranslation(m_Centroid.x, m_Centroid.y, m_Centroid.z);
  m_CentroidNeg_matrix = XMMatrixTranslation(-m_Centroid.x, -m_Centroid.y, -m_Centroid.z);
  m_Translation_matrix = XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
  m_Rotation_matrix = XMMatrixRotationX(m_Rotation.x) * XMMatrixRotationY(m_Rotation.y) * XMMatrixRotationZ(m_Rotation.z);
  m_Scale_matrix = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);

  m_World_matrix = XMMatrixTranspose(m_Centroid_matrix * m_Scale_matrix * m_Rotation_matrix *  m_Translation_matrix);
}

void Transform::Release()
{
}

void Transform::Awake()
{
}