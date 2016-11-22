#include"VertexBuffer.h"
#include "DXUtil.h"

VertexBuffer::VertexBuffer(unsigned stride, unsigned offset) :
m_Offset(offset), m_Buffer(nullptr), m_Created(false), m_Stride(stride)
{
}

VertexBuffer::~VertexBuffer()
{
  releaseBuffer();
}

void VertexBuffer::createBuffer(const void* mem, unsigned int size)
{
  if (!m_Created && mem && size)
  {
    m_Size = size;
    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = m_Stride * m_Size;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData;
    ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = mem;// vertices;
    HR(m_Direct3DDevice->CreateBuffer(&bd, &InitData, &m_Buffer));
    m_Created = true;
  }
}

void VertexBuffer::attachBuffer()
{
  m_Direct3DDeviceContext->IASetVertexBuffers(0, 1, &m_Buffer, &m_Stride, &m_Offset);
}

void VertexBuffer::releaseBuffer()
{
  if (m_Created)
  {
    Memory::SafeRelease(m_Buffer);
    m_Created = false;
  }
}

ID3D11Buffer* VertexBuffer::getBuffer()const
{
  return m_Buffer;
}

ID3D11Buffer** VertexBuffer::getBufferRef()
{
  return &m_Buffer;
}

bool VertexBuffer::getCreated() const
{
  return m_Created;
}

unsigned int VertexBuffer::getStride()
{
  return m_Stride;
}

unsigned int VertexBuffer::getOffset()
{
  return m_Offset;
}

void VertexBuffer::setStride(unsigned stride)
{
  if (!m_Created)
  {
    m_Stride = stride;
  }
}

void VertexBuffer::setOffset(unsigned offset)
{
  if (!m_Created)
  {
    m_Offset = offset;
  }
}

ID3D11Device* VertexBuffer::m_Direct3DDevice = nullptr;
ID3D11DeviceContext* VertexBuffer::m_Direct3DDeviceContext = nullptr;