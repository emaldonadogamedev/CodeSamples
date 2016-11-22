#include "TextureManager.h"

void TextureManager::addTexture(std::string file_name, bool normal_map, Texture** retTextPtrRef)
{
  const std::string tempStr = std::string(normal_map ? NORMAL_MAP : "") + file_name;
  const std::unordered_map<std::string, Texture*>::iterator it(m_textureCollection.find(tempStr));

  if (it == m_textureCollection.end())
  {
    Texture * temp = new Texture(file_name, normal_map);

    if (!temp)
      return;

    temp->loadTexture();

    m_textureCollection[tempStr] = temp;

    if (retTextPtrRef)
    {
      *retTextPtrRef = temp;
    }
  }
}

void TextureManager::removeTexture(std::string file_name)
{
  const std::unordered_map<std::string, Texture*>::iterator it(
    m_textureCollection.find(file_name));

  if (it != m_textureCollection.end())
  {
    it->second->releaseTexture();
    delete it->second;
    m_textureCollection.erase(it);
  }
}

Texture* TextureManager::getTexture(std::string file_name)
{
  const std::unordered_map<std::string, Texture*>::iterator it(
    m_textureCollection.find(file_name) );

  if (it != m_textureCollection.end())
  {
    return it->second;
  }

  return nullptr;
}

void TextureManager::releaseTextures()
{
  std::unordered_map<std::string, Texture*>::iterator it( 
    m_textureCollection.begin() );

  for (; it != m_textureCollection.end(); it++)
  {
    it->second->releaseTexture();
    delete it->second;
  }

  m_textureCollection.clear();
}

void TextureManager::CrossProduct(XMFLOAT3& v1, XMFLOAT3& v2, XMFLOAT3& result)
{
  XMFLOAT3 r = {
     (v1.y * v2.z) - (v2.y * v1.z),
    -((v1.x * v2.z) - (v2.x * v1.z)),
     (v1.x * v2.y) - (v2.x * v1.y)
  };

  result = r;
}

std::unordered_map<std::string, Texture*> TextureManager::m_textureCollection;
