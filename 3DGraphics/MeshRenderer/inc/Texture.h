#pragma once

#include<string>
#include <DirectXMath.h>

using DirectX::XMFLOAT3;

//forward declarations
struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;
struct ID3D11SamplerState;

enum projectorFunction
{
  PF_None = -1,
  PF_Planar,
  PF_Cylindrical,
  PF_Spherical,
  PF_CubeMapping
};

#define NORMAL_MAP "NORMAL_MAP_"

class Texture
{
protected:
  struct TargaHeader
  {
    unsigned char data1[12];
    unsigned short width;
    unsigned short height;
    unsigned char bpp;
    unsigned char data2;
  };

  bool LoadTarga(const char* file_name,  int& w,  int& h);
  void TargaToNormalMap();
  unsigned char* m_targaImage;
  unsigned char* dataPtr;

public:
  Texture(std::string file = "", bool normal_map = false);
  ~Texture();

  //Getters
  unsigned int getTextureWidth()const;
  unsigned int getTextureHeight()const;
  std::wstring getTextureFile()const;
  ID3D11Texture2D* getTexturePtr()const;
  ID3D11ShaderResourceView* getTextureShdrResView()const;
  ID3D11ShaderResourceView** getTextureShdrResViewRef();
  bool getIsLoaded()const;

  //Setters
  void loadTexture();
  void loadTextureAsNormalMap();
  void releaseTexture();

  projectorFunction m_projFuncType;

protected:
  static void CrossProduct(XMFLOAT3& v1, XMFLOAT3& v2, XMFLOAT3& result);
  static void NormalizeFLOAT3(XMFLOAT3& v);

  std::string m_textureFile;
  std::wstring m_textureFileWSTR;
  int m_width, m_height, bytesPerPixel;

  ID3D11Texture2D* m_texture2D;
  ID3D11ShaderResourceView* m_textureShaderResView;

  bool m_isLoaded;
  bool m_isNormalMap;

  static const std::string m_meshAssetsDir;
  static const std::wstring m_meshAssetsDirWSTR;
};