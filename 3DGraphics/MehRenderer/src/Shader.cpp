#include "Shader.h"
#include<iostream>
#include <fstream>
#include<d3d11.h>
#include<d3dcompiler.h>

Shader::Shader(std::string name, ShaderType type) :m_shaderName(name),
m_type(type), m_pShaderBlob(nullptr), m_compiled(false), m_created(false), 
m_bound(false)
{
}

Shader::~Shader()
{

}

HRESULT Shader::CompileShaderFromFile(std::string szEntryPoint, std::string szShaderModel)
{
  HRESULT hr = S_OK;

  DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
  // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
  // Setting this flag improves the shader debugging experience, but still allows 
  // the shaders to be optimized and to run exactly the way they will run in 
  // the release configuration of this program.
  dwShaderFlags |= D3DCOMPILE_DEBUG;

  // Disable optimizations to further improve shader debugging
  dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

  ID3DBlob* pErrorBlob = nullptr;
  std::wstring _file(m_shaderFile.begin(), m_shaderFile.end());
  hr = D3DCompileFromFile(_file.c_str(), nullptr, nullptr, szEntryPoint.c_str(), szShaderModel.c_str(),
    dwShaderFlags, 0, &m_pShaderBlob, &pErrorBlob);

  if (FAILED(hr))
  {
    if (pErrorBlob)
    {
      char* compileErrors;
      unsigned long bufferSize, i;
      std::ofstream fout;

      // Get a pointer to the error message text buffer.
      compileErrors = (char*)(pErrorBlob->GetBufferPointer());

      // Get the length of the message.
      bufferSize = pErrorBlob->GetBufferSize();

      // Open a file to write the error message to.
      fout.open("shader-error.txt");

      // Write out the error message.
      for (i = 0; i < bufferSize; i++)
      {
        fout << compileErrors[i];
      }

      // Close the file.
      fout.close();

      OutputDebugStringA(reinterpret_cast<const char*>(pErrorBlob->GetBufferPointer()));
      std::cout << reinterpret_cast<const char*>(pErrorBlob->GetBufferPointer());
      pErrorBlob->Release();
    }
    return hr;
  }
  if (pErrorBlob) pErrorBlob->Release();

  return S_OK;
}

ShaderType Shader::getType() const
{
  return m_type;
}

std::string Shader::getName() const
{
  return m_shaderName;
}

bool Shader::getCreated() const
{
  return m_created;
}

bool Shader::getCompiled() const
{
  return m_compiled;
}

bool Shader::getBound()
{
  return m_bound;
}

const std::string Shader::c_Shader_location = "Assets/Shaders/";
ID3D11Device* Shader::m_Direct3DDevice = nullptr;
ID3D11DeviceContext* Shader::m_Direct3DDeviceContext = nullptr;