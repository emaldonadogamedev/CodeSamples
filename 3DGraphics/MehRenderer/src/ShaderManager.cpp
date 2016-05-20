#include"ShaderManager.h"

Shader* ShaderManager::getShader(std::string shader_name)
{
  if ( m_shaderCollection.count(shader_name) )
  {
    return m_shaderCollection[shader_name];
  }

  return nullptr;
}


void ShaderManager::addShader(Shader* shader)
{
  if (shader && !m_shaderCollection.count(shader->getName()) )
  {
    if (!shader->getCompiled())
    {
      shader->compileShader();
    }

    if (!shader->getCreated())
    {
      shader->createShader();
    }

    m_shaderCollection[shader->getName()] = shader;
  }
}

void ShaderManager::addShader(ShaderType shader_type, std::string shader_name)
{
  if (!m_shaderCollection.count(shader_name) )
  {
    Shader* shader = nullptr;
    switch (shader_type)
    {
    case VERTEX:
      shader = new VertexShader(shader_name);
      break;

    case PIXEL:
      shader = new PixelShader(shader_name);
      break;
    }

    shader->compileShader();
    shader->createShader();
    m_shaderCollection[shader_name] = shader;
  }
}

void ShaderManager::reAddShader(std::string shader_name)
{
  if (m_shaderCollection.count(shader_name))
  {
    Shader* temp = m_shaderCollection[shader_name];
    temp->reCompileShader();
  }
}

void ShaderManager::removeShader(Shader* shader)
{
  if (shader && m_shaderCollection.count(shader->getName()))
  {
    m_shaderCollection.erase(shader->getName());
    delete shader;
    shader = 0;
  }
}

void ShaderManager::removeShader(std::string shader_name)
{
  if (m_shaderCollection.count(shader_name))
  {
    delete m_shaderCollection[shader_name];
    m_shaderCollection.erase(shader_name);
  }
}

void ShaderManager::clearShaders()
{
  std::unordered_map<std::string, Shader*>::const_iterator iter = 
    m_shaderCollection.begin();

  for ( ; iter != m_shaderCollection.end(); iter++)
  {
    iter->second->releaseShader();
    delete iter->second;
  }

  m_shaderCollection.clear();
}

std::unordered_map<std::string, Shader*> ShaderManager::m_shaderCollection;

Shader* ShaderManager::m_defaultVertexShader = nullptr;
Shader* ShaderManager::m_defaultPixelShader = nullptr;

VertexShader* ShaderManager::m_currBindedVertexShader = nullptr;
PixelShader* ShaderManager::m_currBindedPixelShader = nullptr;