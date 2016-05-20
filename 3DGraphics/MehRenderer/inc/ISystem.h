#pragma  once

#include"IComponent.h"

enum System_Type
{
  ST_Core,
  ST_Graphics,
  ST_Input,
  ST_System_Count
};

template<typename ComponentType = Component>
class ISystem
{
public:
  ISystem(System_Type type, std::string name):m_Sys_type(type), m_Sys_name(name){}

  const std::string m_Sys_name;
  const System_Type m_Sys_type;

  std::vector<ComponentType> m_Components;

  virtual bool Initialize() = 0;
  virtual void Update(float dt) = 0;
  virtual void ShutDown() = 0;

  //Component managers
  virtual void AddComponent(const ComponentType comp) = 0;
  virtual void RemoveComponent(const ComponentType comp) = 0;
};