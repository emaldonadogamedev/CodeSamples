#pragma once

#include <string>

//forward declarations
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D10Blob;

enum ShaderType
{
  None = -1,
  VERTEX,
  PIXEL,
  GEOMETRY,
  ShaderTypeCount
};

//typedefs
typedef long HRESULT;

class Shader
{
public:
  Shader(std::string name = "", ShaderType type = None);
  virtual ~Shader();

  //Methods
  virtual void createShader() = 0;
  virtual void releaseShader() = 0;
  virtual void compileShader() = 0;
  virtual void reCompileShader() = 0;
  virtual void bindShader() = 0;

  //Getters
  ShaderType getType()const;
  std::string getName()const;
  bool getCreated()const;
  bool getCompiled()const;
  bool getBound();
  //Protected methods
protected:
  HRESULT CompileShaderFromFile(std::string szEntryPoint, std::string szShaderModel);

  //Protected members
protected:
  std::string m_shaderName;
  std::string m_shaderFile;
  std::string m_entryPoint;
  std::string m_shaderModel;
  ID3D10Blob* m_pShaderBlob;

  ShaderType m_type;
  bool m_compiled;
  bool m_created;
  bool m_bound;

public:
  static const std::string c_Shader_location;
  static ID3D11Device* m_Direct3DDevice;
  static ID3D11DeviceContext* m_Direct3DDeviceContext;
};