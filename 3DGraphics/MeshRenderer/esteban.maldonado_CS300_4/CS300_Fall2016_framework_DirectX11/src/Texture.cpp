#include "Texture.h"
#include "GraphicsManager.h"
#include "DXUtil.h"

bool Texture::LoadTarga(const char* file_name,  int& w,  int& h)
{
  int error, imageSize;
  FILE* filePtr;
  unsigned int count;
  TargaHeader targaFileHeader;
  unsigned char* tempTargaImage = nullptr;

  // Open the targa file for reading in binary.
  error = fopen_s(&filePtr, file_name, "rb");
  if (error)
    return false;

  // Read in the file header.
  count = fread(&targaFileHeader, sizeof(TargaHeader), 1, filePtr);
  if (count != 1)
    return false;

  // Get the important information from the header.
  h = targaFileHeader.height;
  w = targaFileHeader.width;
  bytesPerPixel = targaFileHeader.bpp / 8;

  // Calculate the size of the 32 bit image data.
  imageSize = w * h * bytesPerPixel;

  // Allocate memory for the targa image data.
  tempTargaImage = new unsigned char[imageSize];
  if (!tempTargaImage)
  {
    return false;
  }

  // Read in the targa image data.
  count = fread(tempTargaImage, 1, imageSize, filePtr);
  if (count != imageSize)
  {
    return false;
  }

  // Close the file.
  error = fclose(filePtr);
  if (error != 0)
  {
    return false;
  }

  // Allocate memory for the targa destination data.
  this->m_targaImage = new unsigned char[imageSize];
  if (!m_targaImage)
  {
    return false;
  }

  // Initialize the index into the targa destination data array.
  int index = 0;

  // Initialize the index into the targa image data.
  int k = (m_width * m_height * bytesPerPixel) - (m_width * bytesPerPixel);

  // Now copy the targa image data into the targa destination array in the correct order since the targa format is stored upside down.
  for (int j = 0; j < m_height; j++)
  {
    for (int i = 0; i < m_width; i++)
    {
       m_targaImage[index + 0] = tempTargaImage[k + 2];  // Red.
       m_targaImage[index + 1] = tempTargaImage[k + 1];  // Green.
       m_targaImage[index + 2] = tempTargaImage[k + 0];  // Blue
       m_targaImage[index + 3] = tempTargaImage[k + 3];  // Alpha

      // Increment the indexes into the targa data.
      k += bytesPerPixel;
      index += bytesPerPixel;
    }

    // Set the targa image data index back to the preceding row at the beginning of the column since its reading it in upside down.
    k -= (m_width * 8);
  }

  //calculate the normalmap from the height map
  if (m_isNormalMap)
  {
    unsigned char* tempNormalMap = new unsigned char[imageSize];
    if (tempNormalMap)
    {
      const float a = 0.01f;

      const int ImageXBytes = m_width * bytesPerPixel;
      const int imageYBytes = m_height * bytesPerPixel;

      const int EndOfImage = imageSize - bytesPerPixel;

      for (int row  = 0; row < m_height; ++row)
      {
        for (int col = 0; col < m_width; ++col)
        {
          int currentIndex = (row * bytesPerPixel * m_width) + (col * bytesPerPixel);
          unsigned char* currentNormalMapPixel = &tempNormalMap[currentIndex];
          
          int heightMapVal1 = m_targaImage[min(currentIndex + (ImageXBytes), EndOfImage)]; // no wrapping, get pixel below
          int heightMapVal2 = m_targaImage[max(currentIndex - (ImageXBytes), 0)];
          XMVECTOR S = { 1.0f, 0,  a * (float)(heightMapVal1 - heightMapVal2), 0 };
          
          heightMapVal1 = m_targaImage[min(currentIndex + bytesPerPixel, EndOfImage)];
          heightMapVal2 = m_targaImage[max(currentIndex - bytesPerPixel, 0)];
          XMVECTOR T = { 0, 1.0f, a * (float)(heightMapVal1 - heightMapVal2), 0 };

          XMVECTOR N = XMVector3Normalize(XMVector3Cross(S,T));

          //move the normalized vector from -1 - 1 range to 0 - 255 range
          currentNormalMapPixel[0] = (unsigned char)(255.0f * 0.5f * (N.m128_f32[0] + 1.0f) );
          currentNormalMapPixel[1] = (unsigned char)(255.0f * 0.5f * (N.m128_f32[1] + 1.0f) );
          currentNormalMapPixel[2] = (unsigned char)(255.0f * 0.5f * (N.m128_f32[2] + 1.0f) );
          currentNormalMapPixel[3] = 255;
        }
      }

      delete [] m_targaImage;
      m_targaImage = tempNormalMap;
    }
  }
  
  // Release the targa image data now that it was copied into the destination array.
  delete[] tempTargaImage;
  tempTargaImage = 0;

  return true;
}

Texture::Texture(std::string file, bool normal_map) :m_textureFile(file), m_texture2D(nullptr),
m_textureShaderResView(nullptr), m_targaImage(nullptr), m_projFuncType(PF_Planar),
    m_isLoaded(false), m_isNormalMap(normal_map)
{
  if (normal_map)
    file = std::string(NORMAL_MAP) + file;

  m_textureFileWSTR.resize(file.size());
  for (int i = 0; i < file.size(); ++i)
    m_textureFileWSTR[i] = file[i];
}

Texture::~Texture()
{
  if (m_targaImage)
  {
    delete m_targaImage;
    m_targaImage = 0;
  }
}

unsigned int Texture::getTextureWidth() const
{
  return m_width;
}

unsigned int Texture::getTextureHeight() const
{
  return m_height;
}

std::wstring Texture::getTextureFile() const
{
  return m_textureFileWSTR;
}

ID3D11Texture2D* Texture::getTexturePtr() const
{
  return m_texture2D;
}

ID3D11ShaderResourceView* Texture::getTextureShdrResView()const
{
  return m_textureShaderResView;
}

ID3D11ShaderResourceView** Texture::getTextureShdrResViewRef()
{
  return &m_textureShaderResView;
}

bool Texture::getIsLoaded() const
{
  return m_isLoaded;
}


void* Texture::getTextureDataPtr()
{
  return (void*)m_targaImage;
}

void Texture::loadTextureNoShdrResView()
{
  if (m_isLoaded)
    return;

  std::string tempFileDir(m_meshAssetsDir + m_textureFile);
  LoadTarga(tempFileDir.c_str(), m_width, m_height);
}

void Texture::loadTexture()
{
  if (m_isLoaded)
    return;

  D3D11_MAPPED_SUBRESOURCE mappedResource;
  D3D11_TEXTURE2D_DESC textureDesc;
  D3D11_SHADER_RESOURCE_VIEW_DESC srDesc;

  ID3D11Device* device = GraphicsManager::GetInstance()->GetDevice();
  ID3D11DeviceContext* context;
  device->GetImmediateContext(&context);

  const float a = 1.0f;

  std::string tempFileDir(m_meshAssetsDir + m_textureFile);

  if (LoadTarga(tempFileDir.c_str(), m_width, m_height))
  {
    // Setup the description of the texture.
    textureDesc.Height = m_height;
    textureDesc.Width = m_width;
    textureDesc.MipLevels = 0;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

    HR(device->CreateTexture2D(&textureDesc, NULL, &m_texture2D));

    // Set the row pitch of the targa image data.
    int rowPitch = (m_width * bytesPerPixel) * sizeof(unsigned char);

    // Copy the targa image data into the texture.
    context->UpdateSubresource(m_texture2D, 0, NULL, m_targaImage, rowPitch, 0);

    // Release the targa image data now that it has been loaded into the texture.
    delete[] m_targaImage;
    m_targaImage = 0;

    // Create the shader-resource view
    srDesc.Format = textureDesc.Format;
    srDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srDesc.Texture2D.MostDetailedMip = 0;
    srDesc.Texture2D.MipLevels = -1;

    // Create the shader resource view for the texture.
    HR(device->CreateShaderResourceView(m_texture2D, &srDesc, &m_textureShaderResView));

    // Generate mipmaps for this texture.
    context->GenerateMips(m_textureShaderResView);

    m_isLoaded = true;

  }
}

void Texture::releaseTexture()
{
  Memory::SafeRelease(m_texture2D);
  Memory::SafeRelease(m_textureShaderResView);
  Memory::SafeDeleteArr(m_targaImage);
  m_isLoaded = false;
}

void Texture::CrossProduct(XMFLOAT3& v1, XMFLOAT3& v2, XMFLOAT3& result)
{
  XMFLOAT3 r = {
    (v1.y * v2.z) - (v2.y * v1.x),
    -((v1.x * v2.z) - (v2.x * v1.z)),
    (v1.x * v2.y) - (v2.x * v1.y)
  };

  result = r;
}

void Texture::NormalizeFLOAT3(XMFLOAT3& v)
{
  const float l = std::sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));

  if (l != 0)
  {
    v.x /= l;
    v.y /= l;
    v.z /= l;
  }
}

const std::string Texture::m_meshAssetsDir = "..\\..\\assets\\textures\\";
const std::wstring Texture::m_meshAssetsDirWSTR = L"..\\..\\assets\\textures\\";