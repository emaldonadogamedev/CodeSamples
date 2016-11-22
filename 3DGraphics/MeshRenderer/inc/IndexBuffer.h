#pragma once

//Forward Declarations
struct ID3D11Buffer;
struct ID3D11Device;
struct ID3D11DeviceContext;

class IndexBuffer
{
public:
  IndexBuffer(unsigned offset = 0);
  ~IndexBuffer();

  //Methods
  void createBuffer(const unsigned int* mem, unsigned int indices_amount);
  void attachBuffer();
  void releaseBuffer();

  //Getters
  ID3D11Buffer* getBuffer()const;
  ID3D11Buffer** getBufferRef();
  bool getCreated()const;

  //Setters
  void setOffset(unsigned offset);

private:
  ID3D11Buffer* m_Buffer;
  unsigned int m_Stride;
  unsigned int m_Size;
  unsigned int m_Offset;
  bool m_Created;

public:
  static ID3D11Device* m_Direct3DDevice;
  static ID3D11DeviceContext* m_Direct3DDeviceContext;
};