#include "IndexBuffer.h"
#include "DXUtil.h"

IndexBuffer::IndexBuffer(unsigned offset) :
  m_Offset(offset), m_Buffer(nullptr),
  m_Created(false), m_Stride(sizeof(UINT))
{
}


IndexBuffer::~IndexBuffer()
{
  releaseBuffer();
}

void IndexBuffer::setOffset(unsigned offset)
{
  m_Offset = offset;
}

bool IndexBuffer::getCreated() const
{
  return m_Created;
}

ID3D11Buffer** IndexBuffer::getBufferRef()
{
  return &m_Buffer;
}


ID3D11Buffer* IndexBuffer::getBuffer() const
{
  return m_Buffer;
}


void IndexBuffer::releaseBuffer()
{
  Memory::SafeRelease(m_Buffer);
  m_Created = false;
}


void IndexBuffer::attachBuffer()
{
  // Set index buffer
  m_Direct3DDeviceContext->IASetIndexBuffer(m_Buffer, DXGI_FORMAT_R32_UINT, 0);
}


void IndexBuffer::createBuffer(const unsigned int* mem, unsigned int indices_amount)
{
  if (!m_Created && mem && indices_amount)
  {
    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(UINT) * indices_amount;// 36 vertices needed for 12 triangles in a triangle list
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData;
    ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = mem;
    HR(m_Direct3DDevice->CreateBuffer(&bd, &InitData, &m_Buffer));

    m_Created = true;
  }
}

ID3D11Device* IndexBuffer::m_Direct3DDevice = nullptr;
ID3D11DeviceContext* IndexBuffer::m_Direct3DDeviceContext = nullptr;