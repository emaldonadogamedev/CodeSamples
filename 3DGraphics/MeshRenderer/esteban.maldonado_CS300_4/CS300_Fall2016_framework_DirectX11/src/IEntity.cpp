#include "IEntity.h"


IEntity::IEntity(GameObject parent):m_Parent(parent),m_Transform(this)
{
  AddComponent(&m_Transform);
}


IEntity::~IEntity()
{
}

void IEntity::AddChild(GameObject go /*= NULL*/)
{
#ifdef _DEBUG
  assert(go != NULL);
#endif // _DEBUG

  if (go)
  {
    m_Children.push_back(go);
  }
}

void IEntity::AddComponent(Component comp /*= NULL*/)
{
#ifdef _DEBUG
  assert(comp != NULL);
#endif // _DEBUG

  if (comp)
  {
    m_Components.push_back(comp);
  }
}

GameObjVec IEntity::FindAllChildrenByName(std::string name) const
{
  GameObjVec vec;
  for (GO_Vec_const_iter it = m_Children.begin(); it != m_Children.end(); it++)
  {
    if ( (*it)->m_GO_name == name)
    {
      vec.push_back(*it);
    }
  }
  return vec;
}

GameObject IEntity::FindChildByName(std::string name) const
{
  for (GO_Vec_const_iter it = m_Children.begin(); it != m_Children.end(); it++)
  {
    if ((*it)->m_GO_name == name)
    {
      return *it;
    }
  }
  return NULL;
}

ComponentVec IEntity::FindAllComponentsByType(ComponentType type)
{
  ComponentVec vec;
  for (Comp_Vec_const_iter it = m_Components.begin(); it != m_Components.end(); it++)
  {
    if ((*it)->m_compType == type)
    {
      vec.push_back(*it);
    }
  }
  return vec;
}

Component IEntity::FindComponentByType(ComponentType type)
{
  for (Comp_Vec_const_iter it = m_Components.begin(); it != m_Components.end(); it++)
  {
    if ((*it)->m_compType == type)
    {
      return *it;
    }
  }
  return NULL;
}
