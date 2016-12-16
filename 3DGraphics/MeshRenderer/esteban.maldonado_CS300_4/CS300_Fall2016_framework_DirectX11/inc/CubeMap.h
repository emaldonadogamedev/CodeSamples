#pragma once

#include<vector>
#include<string.h>

//Forward Declarations
class VertexShader;
class  PixelShader;
class VertexBuffer;
class IndexBuffer;
class Texture;

//D3D forward declarations
struct ID3D11Device;
struct ID3D11RasterizerState;
struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;
struct ID3D11DepthStencilState;

class CubeMap
{
public:
  CubeMap(std::string folder = "mp_classm\\");
  ~CubeMap();

  virtual bool Initialize(ID3D11Device* device);
  virtual void Update(float dt);
  virtual void Render(float dt, ID3D11Device* device, ID3D11RasterizerState * rs);
  virtual void Release();

  //regular buffers
  std::string m_individualFolder;
  unsigned int index_count;
  unsigned int vertex_count;

  //Textures
  std::vector<Texture*> m_cubeMapTextures;

private:
  ID3D11Texture2D* m_pCubeTexture;
  ID3D11ShaderResourceView* m_pShaderResourceView;
  ID3D11DepthStencilState* pDSState;

  void LoadTextures(ID3D11Device* device);

  static VertexShader* s_vertexShader;
  static PixelShader* s_pixelShader;
  static VertexBuffer* s_vertexBuffer;
  static IndexBuffer* s_indexBuffer;
  static const int cubeMapTextureCount;
  static const std::string cubeMapFolder;
  static const std::string s_fileNames[6];
};