#pragma once

//Forward Declarations
struct ID3D11Buffer;
struct ID3D11Device;
struct ID3D11DeviceContext;

class VertexBuffer
{
public:
  VertexBuffer(unsigned  stride, unsigned offset = 0);
  ~VertexBuffer();

  //Methods
  void createBuffer(const void* mem = nullptr, unsigned int size = 0);
  void attachBuffer();
  void releaseBuffer();

  //Getters
  ID3D11Buffer* getBuffer()const;
  ID3D11Buffer** getBufferRef();
  bool getCreated()const;
  unsigned int getStride();
  unsigned int getOffset();

  //Setters
  void setStride(unsigned stride);
  void setOffset(unsigned offset = 0);

  //properties
private:
  ID3D11Buffer* m_Buffer;
  unsigned int m_Stride;
  unsigned int m_Size;
  unsigned int m_Offset;
  bool m_Created;

  //static variables
public:
  static ID3D11Device* m_Direct3DDevice;
  static ID3D11DeviceContext* m_Direct3DDeviceContext;
};