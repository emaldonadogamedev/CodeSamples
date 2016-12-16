#include "PixelShader.h"
#include "DXUtil.h"

using Memory::SafeRelease;

PixelShader::PixelShader(std::string name) :Shader(name, PIXEL), m_pixelShader(nullptr)
{
  m_shaderFile = c_PS_location + name + ".hlsl";
  m_entryPoint = c_PS_EntryPoint;
  m_shaderModel = c_PS_ShaderModel;
}

PixelShader::~PixelShader()
{
  releaseShader();
}

void PixelShader::createShader()
{
  if (!m_compiled)
    compileShader();

  HR(Shader::m_Direct3DDevice->CreatePixelShader(m_pShaderBlob->GetBufferPointer(),
    m_pShaderBlob->GetBufferSize(), NULL, &m_pixelShader));
}

void PixelShader::releaseShader()
{
  SafeRelease(m_pixelShader);
  SafeRelease(m_pShaderBlob);

  m_compiled = m_created = m_bound = false;
}

void PixelShader::compileShader()
{
  if (!m_compiled)
  {
    //compile shader
    HR(CompileShaderFromFile(m_entryPoint, m_shaderModel));
    m_compiled = true;
  }
}

void PixelShader::reCompileShader()
{
  releaseShader();

  createShader();
}

void PixelShader::bindShader()
{
  m_Direct3DDeviceContext->PSSetShader(m_pixelShader, nullptr, 0);
  m_bound = true;
}

const std::string PixelShader::c_PS_location = "..\\..\\assets\\Shaders\\Pixel_Shaders\\";
const std::string PixelShader::c_PS_EntryPoint = "Pixel_Shader";
const std::string PixelShader::c_PS_ShaderModel = "ps_4_0";