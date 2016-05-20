#pragma once
#include "Transform.h"

class IEntity;

typedef IEntity* GameObject;
typedef std::vector<GameObject> GameObjVec;
typedef GameObjVec::iterator GO_Vec_iter;
typedef GameObjVec::const_iterator GO_Vec_const_iter;

__declspec(align(16)) class IEntity
{
public: 
  IEntity(GameObject parent = NULL);
  ~IEntity();

  ComponentVec m_Components;
  GameObjVec m_Children;
  GameObject m_Parent;

  Transform m_Transform;

  std::string m_GO_name;
  std::string m_GO_tag;

  //Setters
  void AddChild(GameObject go = NULL);
  void AddComponent(Component comp = NULL);

  //Getters
  GameObjVec FindAllChildrenByName(std::string name)const;
  GameObject FindChildByName(std::string name)const;

  ComponentVec FindAllComponentsByType(ComponentType type);
  Component FindComponentByType(ComponentType type);

};

