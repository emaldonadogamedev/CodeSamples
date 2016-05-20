#pragma once

#include <unordered_map>
#include "Texture.h"
#include <DirectXMath.h>

using DirectX::XMFLOAT3;

class TextureManager
{
public: 

  //setters
  static void addTexture(std::wstring file_name, bool normal_map = false, Texture** retTextPtrRef = nullptr);
  static void addTexture(void* data, int width, int height, Texture** retTextPtrRef = nullptr);
  static void CS300addNormalTextreFromHeightMap(std::string file_name, Texture** retTextPtrRef = nullptr);
  static void removeTexture(std::wstring file_name);

  //getters
  static Texture* getTexture(std::wstring file_name);

  static void releaseTextures();

private:
  static void CrossProduct(XMFLOAT3& v1, XMFLOAT3& v2, XMFLOAT3& result);

  static std::unordered_map<std::wstring, Texture*> m_textureCollection;
};

