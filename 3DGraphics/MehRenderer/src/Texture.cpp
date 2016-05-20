#include "Texture.h"
#include "GraphicsManager.h"
#include "DXUtil.h"

bool Texture::LoadTarga(char* file_name, unsigned int& w, unsigned int& h)
{
  int error, bpp, imageSize;
  FILE* filePtr;
  unsigned int count;
  TargaHeader targaFileHeader;

  // Open the targa file for reading in binary.
  error = fopen_s(&filePtr, file_name, "rb");
  if (error)
    return false;

  // Read in the file header.
  count = fread(&targaFileHeader, sizeof(TargaHeader), 1, filePtr);
  if (count != 1)
    return false;

  // Get the important information from the header.
  h = (int)targaFileHeader.height;
  w = (int)targaFileHeader.width;
  bpp = (int)targaFileHeader.bpp / 8;

  // Check that it is 32 bit and not 24 bit.
  //if (bpp != 32)
  //{
  //  return false;
  //}

  // Calculate the size of the 32 bit image data.
  imageSize = w * h * bpp;

  // Allocate memory for the targa image data.
  m_targaImage = new unsigned char[imageSize];
  if (!m_targaImage)
  {
    return false;
  }

  // Read in the targa image data.
  count = fread(m_targaImage, 1, imageSize, filePtr);
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

  return true;
}

Texture::Texture(std::wstring file, bool normal_map) :m_textureFile(file), m_texture2D(nullptr),
  m_textureShaderResView(nullptr), m_targaImage(nullptr), m_projFuncType(PF_Cylindrical),
    m_isLoaded(false), m_isNormalMap(normal_map)
{

}

Texture::~Texture()
{
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
  return m_textureFile;
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

void Texture::loadTexture()
{
  if (m_isLoaded)
    return;

  D3D11_MAPPED_SUBRESOURCE mappedResource;
  D3D11_TEXTURE2D_DESC textureDesc;
  D3D11_SHADER_RESOURCE_VIEW_DESC srDesc;
  DirectX::ScratchImage scratchImg, normalMap;

  ID3D11Device* device = GraphicsManager::GetInstance()->GetDevice();
  ID3D11DeviceContext* context;
  device->GetImmediateContext(&context);

  std::wstring temp(m_meshAssetsDir + m_textureFile);
  HRESULT r = CreateDDSTextureFromFile(device, temp.c_str(), (ID3D11Resource**)&m_texture2D, &m_textureShaderResView);
  //HRESULT r = LoadFromTGAFile(temp.c_str(), nullptr, scratchImg);
  //const Image* const img = scratchImg.GetImage(0, 0, 0);

  ////If this is a normal map then begin modifying the 
  //if (m_isNormalMap)
  //{
  //  ComputeNormalMap(*scratchImg.GetImage(0, 0, 0), CNMAP_CHANNEL_RED, 2.0f, DXGI_FORMAT_R8G8B8A8_UNORM, normalMap);

  //  SaveToTGAFile(*normalMap.GetImage(0,0,0), L"TextureNormalTest");
  //}

  //DirectX::ScratchImage& refImg = m_isNormalMap ? normalMap : scratchImg;


  //// Setup the description of the texture.
  //textureDesc.Height = m_height = refImg.GetImage(0, 0, 0)->height;
  //textureDesc.Width = m_width = refImg.GetImage(0, 0, 0)->width;
  //textureDesc.MipLevels = 1;
  //textureDesc.ArraySize = 1;
  //textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  //textureDesc.SampleDesc.Count = 1;
  //textureDesc.SampleDesc.Quality = 0;
  //textureDesc.Usage = D3D11_USAGE_DYNAMIC;
  //textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
  //textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  //textureDesc.MiscFlags = 0;

  //HR(device->CreateTexture2D(&textureDesc, NULL, &m_texture2D));

  //HR(context->Map(m_texture2D, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));
  //
  //std::memcpy(mappedResource.pData, refImg.GetPixels(), refImg.GetPixelsSize());

  //// Unlock the texture.
  //context->Unmap(m_texture2D, 0);

  m_isLoaded = true;

  return;

  const float a = 1.0f;
  if (LoadTarga((char*)(m_meshAssetsDir + m_textureFile).c_str(), m_width, m_height))
  {
    unsigned int i, j, k, rowStart, columnStart;

    // Setup the description of the texture.
    textureDesc.Height = m_height;
    textureDesc.Width = m_width;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage = D3D11_USAGE_DYNAMIC;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    textureDesc.MiscFlags = 0;


    HR(device->CreateTexture2D(&textureDesc, NULL, &m_texture2D));

    HR(context->Map(m_texture2D, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));

    // Get a pointer to the mapped resource data pointer and cast it into an unsigned char to match the targa data.
    dataPtr = (unsigned char*)mappedResource.pData;

    // Initialize the index into the targa data since targa data is stored upside down.
    k = (m_width * m_height * 4) - (m_width * 4);

    // Load the targa data into the texture now.
    for (i = 0; i < m_height; i++)
    {
      // Set the beginning of the row.
      rowStart = i * mappedResource.RowPitch;

      for (j = 0; j < m_width; j++)
      {
        // Set the beginning of the column.
        columnStart = j * 4;

        if (m_isNormalMap)
        {
          XMFLOAT3 S = { 1, 0, 0 };
          XMFLOAT3 T = { 0, 1, 0 };
          XMFLOAT3 N = { 0, 0, 0 };
          
          int temp_i = ((i + 1) == m_height ? i : i + 1);
          int temp_i2 = ((i - 1) == -1 ? i : i - 1);
          S.z = a * (m_targaImage[temp_i * m_width * 3 + (j * 3)] -
            m_targaImage[ temp_i2 * m_width + (j * 3)]);

          int temp_j = ((j + 1) == m_width ? j : j + 1);
          int temp_j2 = ((j - 1) == -1 ? j : j - 1);
          T.z = a * (m_targaImage[ (i * m_width * 3) + ( temp_j * 3) ] - 
            m_targaImage[(i * m_width * 3) + temp_j2 * 3]);
          
          CrossProduct(S, T, N);
          NormalizeFLOAT3(N);

          dataPtr[rowStart + columnStart + 0] = (unsigned char)(255 * 0.5f * (N.x + 1)); // Red.
          dataPtr[rowStart + columnStart + 1] = (unsigned char)(255 * 0.5f * (N.y + 1)); // Green.
          dataPtr[rowStart + columnStart + 2] = (unsigned char)(255 * 0.5f * (N.z + 1)); // Blue
          dataPtr[rowStart + columnStart + 3] = 255;// m_targaImage[k + 3]; // Alpha
        }
        else
        {
          dataPtr[rowStart + columnStart + 0] = m_targaImage[k + 0]; // Red.
          dataPtr[rowStart + columnStart + 1] = m_targaImage[k + 1]; // Green.
          dataPtr[rowStart + columnStart + 2] = m_targaImage[k + 2]; // Blue
          dataPtr[rowStart + columnStart + 3] = 255;// m_targaImage[k + 3]; // Alpha
        }
        // Increment the index into the targa data.
        k += 3;
      }

      // Set the targa data index back to the preceding row at the beginning of the column since its reading it in upside down.
      k -= (m_width * 6); //8
    }

    // Unlock the texture.
    context->Unmap(m_texture2D, 0);

    // Release the targa image data now that it has been loaded into the texture.
    delete[] m_targaImage;
    m_targaImage = 0;

    // Create the shader-resource view
    srDesc.Format = textureDesc.Format;
    srDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srDesc.Texture2D.MostDetailedMip = 0;
    srDesc.Texture2D.MipLevels = 1;

    // Create the shader resource view for the texture.
    HR(device->CreateShaderResourceView(m_texture2D, &srDesc, &m_textureShaderResView));

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

    return;
  }

  ///v.x = v.y = v.z = 0;
}

const std::wstring Texture::m_meshAssetsDir = L"..\\..\\assets\\textures\\";