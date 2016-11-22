#pragma once

#include "Shader.h"

//forward declarations
struct ID3D11PixelShader;

class PixelShader : public Shader
{
public:
  PixelShader(std::string name);
  virtual ~PixelShader() override;
  
  virtual void compileShader() override;
  virtual void reCompileShader() override;
  virtual void createShader() override;
  virtual void bindShader() override;

  virtual void releaseShader() override;

private:
  ID3D11PixelShader* m_pixelShader;

  static const std::string c_PS_location;
  static const std::string c_PS_EntryPoint;
  static const std::string c_PS_ShaderModel;
};