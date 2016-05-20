#include "VertexShader.h"
#include "DXUtil.h"

VertexShader::VertexShader(std::string name) : Shader(name, VERTEX),
m_inputLayoutPtr(nullptr), m_vertexShader(nullptr)
{
  m_shaderFile = c_VS_location + name + ".hlsl";
  m_entryPoint = c_VS_EntryPoint;
  m_shaderModel = c_VS_ShaderModel;
}

VertexShader::~VertexShader()
{
  releaseShader();
}

void VertexShader::createShader()
{
  if (!m_compiled)
    compileShader();

  //create the vertex shader
  HR(Shader::m_Direct3DDevice->CreateVertexShader(m_pShaderBlob->GetBufferPointer(),
    m_pShaderBlob->GetBufferSize(), nullptr, &m_vertexShader));

  // Define the input layout
  D3D11_INPUT_ELEMENT_DESC layout[] =
  {
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },

    { "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "UV_Cylindrical", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "UV_Spherical", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 },

    { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TANGENT_Cylindrical", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TANGENT_Spherical", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },

    { "BI_TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "BI_TANGENT_Cylindrical", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "BI_TANGENT_Spherical", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },

    { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 }
  };
  UINT numElements = ARRAYSIZE(layout);

  HR(m_Direct3DDevice->CreateInputLayout(layout, numElements,m_pShaderBlob->GetBufferPointer(),
    m_pShaderBlob->GetBufferSize(), &m_inputLayoutPtr));

  m_created = true;
}

void VertexShader::releaseShader()
{
  Memory::SafeRelease(m_inputLayoutPtr);
  Memory::SafeRelease(m_vertexShader);
  Memory::SafeRelease(m_pShaderBlob);

  m_compiled = m_created = m_bound = false;
}

void VertexShader::compileShader()
{
  if (!m_compiled)
  {
    //compile shader
    HR(CompileShaderFromFile(m_entryPoint, m_shaderModel));
    m_compiled = true;
  }
}

void VertexShader::reCompileShader()
{
  releaseShader();

  createShader();

  m_compiled = m_created = m_bound = false;
}

void VertexShader::bindShader()
{
  if (m_inputLayoutPtr && m_vertexShader)
  {
    // Set the input layout
    m_Direct3DDeviceContext->IASetInputLayout(m_inputLayoutPtr);

    //set the shader
    m_Direct3DDeviceContext->VSSetShader(m_vertexShader, nullptr, 0);
    m_bound = true;
  }
}

void VertexShader::addInputElement(const InputElement& element)
{
  m_inputLayout.push_back(element);
}

void VertexShader::removeInputElement(const InputElement& element)
{
  for (InputLayout::const_iterator i = m_inputLayout.begin(); 
    i != m_inputLayout.end(); i++)
  {
    if (i->semantic == element.semantic)
    {
      m_inputLayout.erase(i);
    }
  }
}

InputLayout& VertexShader::getInputLayout()
{
  return m_inputLayout;
}

ID3D11InputLayout* VertexShader::getD3DInputLayoutPtr()
{
  return m_inputLayoutPtr;
}

const std::string VertexShader::c_VS_location = "..\\..\\assets\\Shaders\\Vertex_Shaders\\";
const std::string VertexShader::c_VS_EntryPoint = "Vertex_Shader";
const std::string VertexShader::c_VS_ShaderModel = "vs_4_0";