#include "TextureManager.h"

void TextureManager::addTexture(std::wstring file_name, bool normal_map, Texture** retTextPtrRef)
{
  if (!m_textureCollection.count(std::wstring( normal_map ? NORMAL_MAP : L"") +  file_name))
  {
    Texture * temp = new Texture(file_name, normal_map);

    if (!temp)
      return;

    temp->loadTexture();

    m_textureCollection[std::wstring(normal_map ? NORMAL_MAP : L"") + file_name] = temp;

    if (retTextPtrRef)
    {
      *retTextPtrRef = temp;
    }
  }
}

void TextureManager::removeTexture(std::wstring file_name)
{
  if (m_textureCollection.count(file_name))
  {
    m_textureCollection[file_name]->releaseTexture();
    delete m_textureCollection[file_name];
    m_textureCollection.erase(file_name);
  }
}

Texture* TextureManager::getTexture(std::wstring file_name)
{
  if (m_textureCollection.count(file_name))
  {
    return m_textureCollection[file_name];
  }

  return nullptr;
}

void TextureManager::releaseTextures()
{
  std::unordered_map<std::wstring, Texture*>::iterator it( m_textureCollection.begin() );

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
     (v1.y * v2.z) - (v2.y * v1.x),
    -((v1.x * v2.z) - (v2.x * v1.z)),
     (v1.x * v2.y) - (v2.x * v1.y)
  };

  result = r;
}

std::unordered_map<std::wstring, Texture*> TextureManager::m_textureCollection;
