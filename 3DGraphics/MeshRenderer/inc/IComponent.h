#pragma once

enum ComponentType
{
  CT_TRANSFORM,
  CT_SPRITE,
  CT_STATIC_MESH,
  CT_PARTICLE_SYSTEM,
  CT_COUNT
};

#include <vector>
#include<string>

class IComponent;
class IEntity;

typedef IComponent* Component;
typedef std::vector<Component> ComponentVec;
typedef ComponentVec::iterator Comp_Vec_iter;
typedef ComponentVec::const_iterator Comp_Vec_const_iter;

class IComponent
{
public:
  IComponent(const ComponentType type, IEntity* owner, const bool active = true) 
    :m_compType(type), m_Owner(owner), m_IsActive(active)
  {
  }
  virtual ~IComponent(){};

  virtual void Awake() = 0;
  virtual bool Initialize() = 0;
  
  //Update functions
  virtual void FixedUpdate() = 0;
  virtual void Update(float dt) = 0;
  virtual void LateUpdate() = 0;

  virtual void Release() = 0;

  IEntity* m_Owner;
  const ComponentType m_compType;
  bool m_IsActive;
};