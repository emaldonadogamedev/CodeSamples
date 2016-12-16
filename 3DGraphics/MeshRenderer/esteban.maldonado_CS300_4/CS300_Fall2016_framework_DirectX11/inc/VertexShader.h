#pragma once

#include "Shader.h"
#include "GFXTypes.h"

//forward declarations
struct ID3D11InputLayout;
struct ID3D11VertexShader;

class VertexShader : public Shader
{
public:
  VertexShader(std::string name);
  virtual ~VertexShader() override;

  //inherited methods
  virtual void createShader() override;
  virtual void releaseShader() override;
  virtual void compileShader() override;
  virtual void reCompileShader() override;
  virtual void bindShader() override;

  //setters
  void addInputElement(const InputElement& element);
  void removeInputElement(const InputElement& element);

  //getters
  InputLayout& getInputLayout();
  ID3D11InputLayout* getD3DInputLayoutPtr();

private:
  InputLayout m_inputLayout;
  ID3D11InputLayout* m_inputLayoutPtr;
  ID3D11VertexShader* m_vertexShader;

public:
  static const std::string c_VS_location;
  static const std::string c_VS_EntryPoint;
  static const std::string c_VS_ShaderModel;
};