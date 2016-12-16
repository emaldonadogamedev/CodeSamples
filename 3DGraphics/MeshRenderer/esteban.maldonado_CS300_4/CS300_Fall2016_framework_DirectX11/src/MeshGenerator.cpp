#include "MeshGenerator.h"
#include <set>
#include <float.h>
#include <fstream>
#include <algorithm>
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include <DirectXMath.h>

using DirectX::XMVECTOR;
using DirectX::XM_PI;
using DirectX::XMMATRIX;
using DirectX::XMMatrixScaling;
using DirectX::XMMatrixTranslation;

VertexBuffer* MeshGenerator::getObjVertexBuffer(std::string objName)
{
  const std::unordered_map<std::string, MeshBuffer>::iterator it = 
    m_meshBufferCollection.find(objName);

  if (it != m_meshBufferCollection.end())
  {
    return it->second.vBuffer;
  }
  return nullptr;
}

VertexBuffer* MeshGenerator::getObjVertexVNormalBuffer(std::string objName)
{
  const std::unordered_map<std::string, MeshBuffer>::iterator it =
    m_meshBufferCollection.find(objName + DEBUG_VERTEX_NORMAL);

  if (it != m_meshBufferCollection.end())
  {
    return it->second.vBuffer;
  }

  return nullptr;
}

VertexBuffer* MeshGenerator::getObjVertexFaceNormalBuffer(std::string objName)
{
  const std::unordered_map<std::string, MeshBuffer>::iterator it =
    m_meshBufferCollection.find(objName + DEBUG_FACE_NORMAL);

  if (it != m_meshBufferCollection.end())
  {
    return it->second.vBuffer;
  }

  return nullptr;
}

IndexBuffer* MeshGenerator::getObjIndexBuffer(std::string objName)
{
  const std::unordered_map<std::string, MeshBuffer>::iterator it(
    m_meshBufferCollection.find(objName) );

  if (it != m_meshBufferCollection.end())
  {
    return it->second.iBuffer;
  }
  return nullptr;
}

MeshBuffer* MeshGenerator::getObjMeshBuffer(std::string objName)
{
  const std::unordered_map<std::string, MeshBuffer>::iterator it(
    m_meshBufferCollection.find(objName) );

  if (it != m_meshBufferCollection.end())
  {
    return &it->second;// &m_meshBufferCollection[objName];
  }
  return nullptr;
}

MeshData* MeshGenerator::getObjMeshDataPtr(std::string objName)
{
  const std::unordered_map<std::string, MeshData>::iterator it(
    m_meshCollection.find(objName) );

  if (it != m_meshCollection.end())
  {
    return  &it->second;
  }
  return nullptr;
}

MeshBuffer MeshGenerator::CreateBox(MeshData& meshData, std::string name, float width /*= 1.0f*/, float height /*= 1.0f*/, float depth /*= 1.0f*/, XMFLOAT4 color)
{
  meshData.Vertices.clear();
  meshData.Indices.clear();
  meshData.Faces.clear();

  const std::unordered_map<std::string, MeshData>::iterator it(
    m_meshCollection.find(name));

  if ( it != m_meshCollection.end() )
  {
    meshData = it->second;

    return m_meshBufferCollection[name];
  }

  VertexTexture v[24];

  const float w2 = 0.5f*width;
  const float h2 = 0.5f*height;
  const float d2 = 0.5f*depth;
  unsigned int faceCount = 0;

  // Fill in the front face vertex data.
  v[0] = VertexTexture(-w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, color);
  v[1] = VertexTexture(-w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, color);
  v[2] = VertexTexture(+w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, color);
  v[3] = VertexTexture(+w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, color);

  // Fill in the back face vertex data.
  v[4] = VertexTexture(-w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, color);
  v[5] = VertexTexture(+w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, color);
  v[6] = VertexTexture(+w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, color);
  v[7] = VertexTexture(-w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, color);

  // Fill in the top face vertex data.
  v[8]  = VertexTexture(-w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, color);
  v[9]  = VertexTexture(-w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, color);
  v[10] = VertexTexture(+w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, color);
  v[11] = VertexTexture(+w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, color);

  // Fill in the bottom face vertex data.
  v[12] = VertexTexture(-w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, color);
  v[13] = VertexTexture(+w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, color);
  v[14] = VertexTexture(+w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, color);
  v[15] = VertexTexture(-w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, color);

  // Fill in the left face vertex data.
  v[16] = VertexTexture(-w2, -h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, color);
  v[17] = VertexTexture(-w2, +h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, color);
  v[18] = VertexTexture(-w2, +h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, color);
  v[19] = VertexTexture(-w2, -h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, color);

  // Fill in the right face vertex data.
  v[20] = VertexTexture(+w2, -h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, color);
  v[21] = VertexTexture(+w2, +h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, color);
  v[22] = VertexTexture(+w2, +h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, color);
  v[23] = VertexTexture(+w2, -h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, color);

  meshData.Vertices.assign(&v[0], &v[24]);

  MeshFace tempFace;
  //
  // Create the indices.
  //
  unsigned int i[36];

  // Fill in the front face index data
  i[0] = 0; i[1] = 1; i[2] = 2;
  tempFace.faceID = faceCount++;
  CrossProduct(meshData.Vertices[0].position, meshData.Vertices[1].position, tempFace.normal);
  meshData.Faces.push_back(tempFace);

  i[3] = 0; i[4] = 2; i[5] = 3;
  tempFace.faceID = faceCount++;
  CrossProduct(meshData.Vertices[0].position, meshData.Vertices[2].position, tempFace.normal);
  meshData.Faces.push_back(tempFace);

  // Fill in the back face index data
  i[6] = 4; i[7] = 5; i[8] = 6;
  tempFace.faceID = faceCount++;
  CrossProduct(meshData.Vertices[4].position, meshData.Vertices[5].position, tempFace.normal);
  meshData.Faces.push_back(tempFace);

  i[9] = 4; i[10] = 6; i[11] = 7;
  tempFace.faceID = faceCount++;
  CrossProduct(meshData.Vertices[4].position, meshData.Vertices[6].position, tempFace.normal);
  meshData.Faces.push_back(tempFace);

  // Fill in the top face index data
  i[12] = 8; i[13] = 9; i[14] = 10;
  tempFace.faceID = faceCount++;
  CrossProduct(meshData.Vertices[8].position, meshData.Vertices[9].position, tempFace.normal);
  meshData.Faces.push_back(tempFace);

  i[15] = 8; i[16] = 10; i[17] = 11;
  tempFace.faceID = faceCount++;
  CrossProduct(meshData.Vertices[8].position, meshData.Vertices[10].position, tempFace.normal);
  meshData.Faces.push_back(tempFace);

  // Fill in the bottom face index data
  i[18] = 12; i[19] = 13; i[20] = 14;
  tempFace.faceID = faceCount++;
  CrossProduct(meshData.Vertices[12].position, meshData.Vertices[13].position, tempFace.normal);
  meshData.Faces.push_back(tempFace);

  i[21] = 12; i[22] = 14; i[23] = 15;
  tempFace.faceID = faceCount++;
  CrossProduct(meshData.Vertices[12].position, meshData.Vertices[14].position, tempFace.normal);
  meshData.Faces.push_back(tempFace);

  // Fill in the left face index data
  i[24] = 16; i[25] = 17; i[26] = 18;
  tempFace.faceID = faceCount++;
  CrossProduct(meshData.Vertices[16].position, meshData.Vertices[17].position, tempFace.normal);
  meshData.Faces.push_back(tempFace);

  i[27] = 16; i[28] = 18; i[29] = 19;
  tempFace.faceID = faceCount++;
  CrossProduct(meshData.Vertices[16].position, meshData.Vertices[18].position, tempFace.normal);
  meshData.Faces.push_back(tempFace);

  // Fill in the right face index data
  i[30] = 20; i[31] = 21; i[32] = 22;
  tempFace.faceID = faceCount++;
  CrossProduct(meshData.Vertices[20].position, meshData.Vertices[21].position, tempFace.normal);
  meshData.Faces.push_back(tempFace);

  i[33] = 20; i[34] = 22; i[35] = 23;
  tempFace.faceID = faceCount++;
  CrossProduct(meshData.Vertices[20].position, meshData.Vertices[22].position, tempFace.normal);
  meshData.Faces.push_back(tempFace);

  meshData.Indices.assign(&i[0], &i[36]);

  m_meshCollection[name] = meshData;
  
  VertexBuffer* vBuffer = new VertexBuffer(sizeof(VertexTexture), 0);
  IndexBuffer* iBuffer = new IndexBuffer();

  vBuffer->createBuffer(meshData.Vertices.data(), meshData.Vertices.size());
  iBuffer->createBuffer(meshData.Indices.data(), meshData.Indices.size());

  MeshBuffer result;
  result.vBuffer = vBuffer;
  result.iBuffer = iBuffer;
  result.index_count = 36;
  result.vertex_count = 24;

  m_meshBufferCollection[name] = result;

  return result;
}

MeshBuffer MeshGenerator::CreateQuad(MeshData& meshData, float width, float height, XMFLOAT4 color)
{
  meshData.Vertices.clear();
  meshData.Indices.clear();
  meshData.Faces.clear();

  const std::unordered_map<std::string, MeshData>::iterator it(
    m_meshCollection.find(QUAD));

  if (m_meshCollection.end() != it)
  {
    meshData = it->second;// m_meshCollection[QUAD];

    return m_meshBufferCollection[QUAD];
  }
  
  float w2 = 0.5f * width;
  float h2 = 0.5f * height;
  unsigned int faceCount = 0;
  
  // Fill in the front face vertex data.
  VertexTexture v[4] = 
  {
    VertexTexture(-w2, -h2, 0, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, color),
    VertexTexture(-w2, +h2, 0, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, color),
    VertexTexture(+w2, +h2, 0, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, color),
    VertexTexture(+w2, -h2, 0, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, color)
  };

  meshData.Vertices.assign(v, &v[4]);
  MeshFace tempFace;
  //
  // Create the indices.
  //
  unsigned int i[6] = 
  {
    0, 1, 2,
    0, 2, 3
  };

  // Fill in the front face index data
  tempFace.faceID = faceCount++;
  CrossProduct(meshData.Vertices[0].position, meshData.Vertices[1].position, tempFace.normal);
  meshData.Faces.push_back(tempFace);

  tempFace.faceID = faceCount++;
  CrossProduct(meshData.Vertices[0].position, meshData.Vertices[2].position, tempFace.normal);
  meshData.Faces.push_back(tempFace);

  m_meshCollection[QUAD] = meshData;

  //Create the actual buffers
  VertexBuffer* vBuffer = new VertexBuffer(sizeof(VertexTexture), 0);
  IndexBuffer* iBuffer = new IndexBuffer();

  vBuffer->createBuffer(meshData.Vertices.data(), meshData.Vertices.size());
  iBuffer->createBuffer(meshData.Indices.data(), meshData.Indices.size());

  //prepare the MeshBuffer result
  MeshBuffer result;
  result.vBuffer = vBuffer;
  result.iBuffer = iBuffer;
  result.index_count = meshData.Indices.size();
  result.vertex_count = meshData.Vertices.size();

  //add the new buffers to the collection...
  m_meshBufferCollection[QUAD] = result;

  return result;
}

MeshBuffer MeshGenerator::CreateSphere(MeshData& meshData, float radius, unsigned int sliceCount, unsigned int stackCount)
{
  meshData.Vertices.clear();
  meshData.Indices.clear();
  meshData.Faces.clear();

  const std::unordered_map<std::string, MeshData>::iterator it(
    m_meshCollection.find(SPHERE_MESH));

  if (it != m_meshCollection.end())// m_meshCollection.count(SPHERE_MESH))
  {
    meshData = it->second;

    return m_meshBufferCollection[SPHERE_MESH];
  }

  //
  // Compute the vertices stating at the top pole and moving down the stacks.
  //

  // Poles: note that there will be texture coordinate distortion as there is
  // not a unique point on the texture map to assign to the pole when mapping
  // a rectangular texture onto a sphere.
  VertexTexture topVertex(0.0f, +radius, 0.0f, 0.0f, +1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
  VertexTexture bottomVertex(0.0f, -radius, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);

  meshData.Vertices.push_back(topVertex);

  float phiStep = XM_PI / stackCount;
  float thetaStep = /*2.0f*XM_PI*/ DirectX::XM_2PI / sliceCount;

  // Compute vertices for each stack ring (do not count the poles as rings).
  for (unsigned int i = 1; i <= stackCount - 1; ++i)
  {
    float phi = i*phiStep;

    // Vertices of ring.
    for (unsigned int j = 0; j <= sliceCount; ++j)
    {
      float theta = j*thetaStep;

      VertexTexture v;

      // spherical to cartesian
      v.position.x = radius*sinf(phi)*cosf(theta);
      v.position.y = radius*cosf(phi);
      v.position.z = radius*sinf(phi)*sinf(theta);

      // Partial derivative of P with respect to theta
      v.tangent.x = -radius*sinf(phi)*sinf(theta);
      v.tangent.y = 0.0f;
      v.tangent.z = +radius*sinf(phi)*cosf(theta);

      XMVECTOR T = XMLoadFloat3(&v.tangent);
      XMStoreFloat3(&v.tangent, DirectX::XMVector3Normalize(T));

      XMVECTOR p = DirectX::XMLoadFloat3(&v.position);
      XMStoreFloat3(&v.normal, DirectX::XMVector3Normalize(p));

      v.uv.x = theta / DirectX::XM_2PI;
      v.uv.y = phi / XM_PI;

      meshData.Vertices.push_back(v);
    }
  }

  meshData.Vertices.push_back(bottomVertex);

  //
  // Compute indices for top stack.  The top stack was written first to the vertex buffer
  // and connects the top pole to the first ring.
  //

  for (unsigned int i = 1; i <= sliceCount; ++i)
  {
    meshData.Indices.push_back(0);
    meshData.Indices.push_back(i + 1);
    meshData.Indices.push_back(i);
  }

  //
  // Compute indices for inner stacks (not connected to poles).
  //

  // Offset the indices to the index of the first vertex in the first ring.
  // This is just skipping the top pole vertex.
  unsigned int baseIndex = 1;
  unsigned int ringVertexCount = sliceCount + 1;
  for (unsigned int i = 0; i < stackCount - 2; ++i)
  {
    for (unsigned int j = 0; j < sliceCount; ++j)
    {
      meshData.Indices.push_back(baseIndex + i*ringVertexCount + j);
      meshData.Indices.push_back(baseIndex + i*ringVertexCount + j + 1);
      meshData.Indices.push_back(baseIndex + (i + 1)*ringVertexCount + j);

      meshData.Indices.push_back(baseIndex + (i + 1)*ringVertexCount + j);
      meshData.Indices.push_back(baseIndex + i*ringVertexCount + j + 1);
      meshData.Indices.push_back(baseIndex + (i + 1)*ringVertexCount + j + 1);
    }
  }

  //
  // Compute indices for bottom stack.  The bottom stack was written last to the vertex buffer
  // and connects the bottom pole to the bottom ring.
  //

  // South pole vertex was added last.
  unsigned int southPoleIndex = (unsigned int)meshData.Vertices.size() - 1;

  // Offset the indices to the index of the first vertex in the last ring.
  baseIndex = southPoleIndex - ringVertexCount;

  for (unsigned int i = 0; i < sliceCount; ++i)
  {
    meshData.Indices.push_back(southPoleIndex);
    meshData.Indices.push_back(baseIndex + i);
    meshData.Indices.push_back(baseIndex + i + 1);
  }

  //Add the mesh to the collection...
  m_meshCollection[SPHERE_MESH] = meshData;

  //Create the actual buffers
  VertexBuffer* vBuffer = new VertexBuffer(sizeof(VertexTexture), 0);
  IndexBuffer* iBuffer = new IndexBuffer();

  vBuffer->createBuffer(meshData.Vertices.data(), meshData.Vertices.size());
  iBuffer->createBuffer(meshData.Indices.data(), meshData.Indices.size());

  //prepare the MeshBuffer result
  MeshBuffer result;
  result.vBuffer = vBuffer;
  result.iBuffer = iBuffer;
  result.index_count = meshData.Indices.size();
  result.vertex_count = meshData.Vertices.size();

  //add the new buffers to the collection...
  m_meshBufferCollection[SPHERE_MESH] = result;

  return result;
}


//XMFLOAT3 comparison functor
bool XMF3(const XMFLOAT3& v1, const XMFLOAT3& v2)
{
  if (std::fabs(v1.x - v2.x) > 0.001f || std::fabs(v1.y - v2.y) > 0.001f || std::fabs(v1.z - v2.z) > 0.001f)
  {
    return false;
  }
  return true;
}

MeshBuffer& MeshGenerator::CS300LoadMeshFromOBJ(MeshData& meshData, std::string const &objFile)
{
  std::unordered_map<std::string, MeshData>::iterator const it =
    m_meshCollection.find(objFile);

  if (it != m_meshCollection.end() )//m_meshCollection.count(objFile) > 0)
  {
    meshData.Vertices.clear();
    meshData.Indices.clear();
    meshData.Faces.clear();

    meshData =  it->second;

    return m_meshBufferCollection[objFile];
  }

  std::ifstream input;
  unsigned face_count = -1;
  input.open(m_meshAssetsDir + objFile);
  
  //return false if no model founded
  if (!input.is_open())
    return CreateBox(meshData);

  //continue if file was opened just fine.
  meshData.Indices.clear();
  meshData.Vertices.clear();
  meshData.Faces.clear();

  char buff[1000] = { 0 };
  VertexTexture *tempV1, *tempV2, *tempV3, tempCentroid, tempVertex;
  float tempVextX, tempVextY, tempVextZ;
    tempCentroid.color = XMFLOAT4(0,0,0,0);
  
  XMFLOAT3 sub1, sub2, *tempFaceNormalPtr;
  unsigned int tempI1, tempI2, tempI3;
  MeshFace tempFace;
  std::vector<VertexTexture> vertex_Normals;
  std::vector<VertexTexture> face_Normals;
  const float normal_size_mod = 0.06f;
  VecNormalMap VertexNormalMap;

  bool isLoadingNormals = false;

  float centroidX(0), centroidY(0), centroidZ(0), maxX(FLT_MIN), maxY(FLT_MIN), maxZ(FLT_MIN),
    minX(FLT_MAX), minY(FLT_MAX), minZ(FLT_MAX);

#pragma region Read the model file
  //# 35947 vertices, 0 vertices normals
  while (!input.eof())
  {
    input >> buff;
    switch (*buff)
    {
    case '#':
      //input.getline(buff, 500);
      input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      break;

    case 'v':

      //read in the x, y and z position of the vertex
      input >> tempVextX >> tempVextY >> tempVextZ;

      //Update centroid coordinates
      //X
      centroidX += tempVextX;
      if (tempVextX< minX)
        minX = tempVextX;

      else if (tempVextX > maxX)
        maxX = tempVextX;

      //Y
      centroidY += tempVextY;
      if (tempVextY < minY)
        minY = tempVextY;

      else if (tempVextY > maxY)
        maxY = tempVextY;

      //Z
      centroidZ += tempVextZ;
      if (tempVextZ < minZ)
        minZ = tempVextX;

      else if (tempVextZ > maxZ)
        maxZ = tempVextZ;

      //End - Update centroid coordinates
      tempVertex.position.x = tempVextX;
      tempVertex.position.y = tempVextY;
      tempVertex.position.z = tempVextZ;
      meshData.Vertices.push_back(tempVertex);
      break;

  ////////////////////////////////////////////poop//////////////////////////////
  //BY THIS POINT ALL THE VERTICES HAVE BEEN LOADED
    case 'f':
      if(VertexNormalMap.size() == 0)
        VertexNormalMap.resize(meshData.Vertices.size());

      //read in the triangle face indices(When loaded from the file, they're 1-based)
      input >> tempI1 >> tempI2 >> tempI3;
      meshData.Indices.push_back(--tempI1);
      tempV1 = &meshData.Vertices[tempFace.v1_ID = tempI1];

      meshData.Indices.push_back(--tempI2);
      tempV2 = &meshData.Vertices[tempFace.v2_ID = tempI2];

      meshData.Indices.push_back(--tempI3);
      tempV3 = &meshData.Vertices[tempFace.v3_ID = tempI3];

      //////////////////////////////////////////////////////////////////////////
      //CALCULATE THE FACE NORMAL - 

      //Get the 2 vectors between the 3 points
      SubtractVec(tempV2->position, tempV1->position, sub1);
      SubtractVec(tempV2->position, tempV3->position, sub2);

      //Find the triangle centroid
      FindTriangleCentroid(tempV1->position, tempV2->position,
        tempV3->position, tempCentroid.position);

      tempFace.faceID = ++face_count;

      //Get the cross proudct of the 2 vectors into the normal
      CrossProduct(sub2, sub1, tempFace.normal);
      NormalizeFLOAT3(tempFace.normal);

      //Store the current face on the face vector
      meshData.Faces.push_back(tempFace);

      //store the face normals so we can draw it later on...
      face_Normals.push_back(tempCentroid);
      tempCentroid.position.x -= (tempFace.normal.x * normal_size_mod);
      tempCentroid.position.y -= (tempFace.normal.y * normal_size_mod);
      tempCentroid.position.z -= (tempFace.normal.z * normal_size_mod);
      face_Normals.push_back(tempCentroid);

      // STORE THE VERTEX NORMALS ON THE VECTOR MAP
      tempFaceNormalPtr = &tempFace.normal;
      VertexNormalMap[tempI1].push_back(*tempFaceNormalPtr);
      VertexNormalMap[tempI2].push_back(*tempFaceNormalPtr);
      VertexNormalMap[tempI3].push_back(*tempFaceNormalPtr);

  //////////////////////////////////////////////////////////////////////////
  //END CALCULATE THE FACE NORMAL
      break;

    default:
      input.getline(buff, 500);
      input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      break;
    }
  }

#pragma endregion

  //close the file, we don't need it anymore
  input.close();

  //add the min and max coordinates
  meshData.m_minCoords = XMFLOAT4(minX, minY, minZ, 1.0f);
  meshData.m_maxCoords = XMFLOAT4(maxX, maxY, maxZ, 1.0f);

#pragma region CALCULATE VERTEX NORMALS
  //Prepare to store vertex normals
  vertex_Normals.resize(meshData.Vertices.size() * 2);

  //main for loop
  for (unsigned int i = 0; i < VertexNormalMap.size(); ++i)
  {
    //Remove duplicates if we have more than 1 normal
    if (VertexNormalMap[i].size() > 1)
    {
      const auto& last = std::unique(VertexNormalMap[i].begin(), VertexNormalMap[i].end(), 
        [](const XMFLOAT3& v1, const XMFLOAT3& v2)
        {
          //lambda function that returns whether 2 XMFLOAT3 vectors are equal
          if (std::fabs(v1.x - v2.x) > 0.0001f || std::fabs(v1.y - v2.y) > 0.0001f || std::fabs(v1.z - v2.z) > 0.0001f)
          {
            return false;
          }
          return true;
        }/*XMF3*/
      );

      //remove the normals that are not needed
      VertexNormalMap[i].erase(last, VertexNormalMap[i].end());
    }

    XMFLOAT3 temp_normal(0, 0, 0);
    XMFLOAT3 temp_normalLine;

    for (int j = 0; j < VertexNormalMap[i].size(); ++j )
    {
      temp_normal.x += VertexNormalMap[i][j].x;
      temp_normal.y += VertexNormalMap[i][j].y;
      temp_normal.z += VertexNormalMap[i][j].z;
    }

    temp_normal.x /= (float)VertexNormalMap[i].size();
    temp_normal.y /= (float)VertexNormalMap[i].size();
    temp_normal.z /= (float)VertexNormalMap[i].size();

    //NormalizeFLOAT3(temp_normal);
    
    meshData.Vertices[i].normal = temp_normal;
    
    //Create the vertex buffers for the vertex normals
    vertex_Normals[i].position = meshData.Vertices[i].position;
    temp_normalLine.x = meshData.Vertices[i].position.x - (meshData.Vertices[i].normal.x * normal_size_mod);
    temp_normalLine.y = meshData.Vertices[i].position.y - (meshData.Vertices[i].normal.y * normal_size_mod);
    temp_normalLine.z = meshData.Vertices[i].position.z - (meshData.Vertices[i].normal.z * normal_size_mod);
    vertex_Normals[i + 1].position = temp_normalLine;

  } // end main for loop
#pragma endregion

#pragma region CALCULATE UV COORDINATES
  for (auto& v : meshData.Vertices)
  {
    CalculateUV(v, meshData.m_minCoords, meshData.m_maxCoords);
    CalculateUV_Cylindrical(v, meshData.m_minCoords, meshData.m_maxCoords);
    CalculateUV_Spherical(v, meshData.m_minCoords, meshData.m_maxCoords);
  }
#pragma  endregion

#pragma region CALCULATE TANGENTS & BI_TANGENTS

  //Allocate the collection of tangents and biTangents for planar, cylindrical and Spherical
  VecNormalMap tangentMap, tangentMap_C, tangentMap_S;
  VecNormalMap biTangentMap, biTangentMap_C, biTangentMap_S;

  tangentMap.resize(meshData.Vertices.size()); 
  tangentMap_C.resize(meshData.Vertices.size());
  tangentMap_S.resize(meshData.Vertices.size());

  biTangentMap.resize(meshData.Vertices.size());
  biTangentMap_C.resize(meshData.Vertices.size());
  biTangentMap_S.resize(meshData.Vertices.size());

  //go through all of the triangle faces to calculate the face tangents and biTangents
  for (auto& triangle : meshData.Faces)
  {
    CalculateTangentBinormal(triangle,  meshData.Vertices[triangle.v1_ID], meshData.Vertices[triangle.v2_ID],
      meshData.Vertices[triangle.v3_ID], tangentMap, biTangentMap, tangentMap_C, biTangentMap_C, tangentMap_S, biTangentMap_S);
  }

#pragma region Eliminate duplicate tangents
  for (unsigned int i = 0; i < tangentMap.size(); ++i)
  {
    EliminateDuplicateVector(tangentMap[i], meshData.Vertices[i].tangent);
    EliminateDuplicateVector(tangentMap_C[i], meshData.Vertices[i].tangent_Cylindrical);
    EliminateDuplicateVector(tangentMap_S[i], meshData.Vertices[i].tangent_Spherical);
  }
#pragma  endregion

#pragma region Eliminate duplicate biTangents
  for (unsigned int i = 0; i < biTangentMap.size(); ++i)
  {
    EliminateDuplicateVector(biTangentMap[i], meshData.Vertices[i].biTangent);
    EliminateDuplicateVector(biTangentMap_C[i], meshData.Vertices[i].biTangent_Cylindrical);
    EliminateDuplicateVector(biTangentMap_S[i], meshData.Vertices[i].biTangent_Spherical);
  }
#pragma  endregion

#pragma endregion

  //Add the loaded mesh to the collection
  m_meshCollection[objFile] = meshData;

  //Create the actual buffers for the model
  VertexBuffer* vBuffer = new VertexBuffer( sizeof(VertexTexture), 0);
  IndexBuffer* iBuffer = new IndexBuffer();

  vBuffer->createBuffer(meshData.Vertices.data(), meshData.Vertices.size());
  iBuffer->createBuffer(meshData.Indices.data(), meshData.Indices.size());

  //prepare the MeshBuffer result
  MeshBuffer result, debugR;
  result.vBuffer = vBuffer;
  result.iBuffer = iBuffer;
  result.index_count = meshData.Indices.size();
  result.vertex_count = meshData.Vertices.size();

  result.m_centroid.x = centroidX / result.vertex_count;
  result.m_centroid.y = centroidY / result.vertex_count;
  result.m_centroid.z = centroidZ / result.vertex_count;

  //add the new buffers to the collection...
  m_meshBufferCollection[objFile] = result;

  //////////////////////////////////////////////////////////////////////////
  //Create the vertex normal buffer
  vBuffer = new VertexBuffer(sizeof(VertexTexture), 0);
  vBuffer->createBuffer(vertex_Normals.data(), vertex_Normals.size());
  //prepare the result
  debugR.vBuffer = vBuffer;
  debugR.iBuffer = nullptr;
  debugR.index_count = 0;
  debugR.vertex_count = vertex_Normals.size();

  debugR.m_centroid = result.m_centroid;

  //store the vbuffer of the vertex normals
  m_meshBufferCollection[objFile + DEBUG_VERTEX_NORMAL] = debugR;
  
  //////////////////////////////////////////////////////////////////////////
  //Create the vertex face normal buffer
  vBuffer = new VertexBuffer(sizeof(VertexTexture), 0);
  vBuffer->createBuffer(face_Normals.data(), face_Normals.size());
  //prepare the result
  debugR.vBuffer = vBuffer;
  debugR.iBuffer = nullptr;
  debugR.index_count = 0;
  debugR.vertex_count = face_Normals.size();

  debugR.m_centroid = result.m_centroid;

  //store the vbuffer of the vertex normals
  m_meshBufferCollection[objFile + DEBUG_FACE_NORMAL] = debugR;


  //Calculate Tangents
  return m_meshBufferCollection[objFile];// result;
}

void MeshGenerator::ReleaseMeshBuffers()
{
  std::unordered_map<std::string, MeshBuffer>::iterator it;
  for (it = m_meshBufferCollection.begin(); it != m_meshBufferCollection.end(); ++it)
  {
    if (it->second.vBuffer)
      it->second.vBuffer->releaseBuffer();
    
    if (it->second.iBuffer)
      it->second.iBuffer->releaseBuffer();    
  }
}

VertexBuffer* MeshGenerator::m_currBindedVertexBuffer = nullptr;
IndexBuffer* MeshGenerator::m_currBindedIndexBuffer = nullptr;

const std::string MeshGenerator::m_meshAssetsDir = "..\\..\\assets\\models\\";

std::unordered_map<std::string, MeshData> MeshGenerator::m_meshCollection;

std::unordered_map<std::string, MeshBuffer> MeshGenerator::m_meshBufferCollection;

void MeshGenerator::CrossProduct(XMFLOAT3& v1, XMFLOAT3& v2, XMFLOAT3& result)
{
  XMFLOAT3 r;
  
  r.x =  (v1.y * v2.z) - (v2.y * v1.z);
  r.y = -((v1.x * v2.z) - (v2.x * v1.z));
  r.z =  (v1.x * v2.y) - (v2.x * v1.y);
 
  result = r;
}

void MeshGenerator::SubtractVec(XMFLOAT3& v1, XMFLOAT3& v2, XMFLOAT3& result)
{
  result.x = v1.x - v2.x;
  result.y = v1.y - v2.y;
  result.z = v1.z - v2.z;
}

void MeshGenerator::NormalizeFLOAT3(XMFLOAT3& v)
{
  const float l = std::sqrt( (v.x * v.x) + (v.y * v.y) + (v.z * v.z));

  if (l != 0)
  {
    v.x /= l;
    v.y /= l;
    v.z /= l;

    return;
  }

  v.x = v.y = v.z = 0.f;
}

void MeshGenerator::FindTriangleCentroid(const XMFLOAT3& p1, const XMFLOAT3& p2, 
  const XMFLOAT3& p3, XMFLOAT3& result)
{
  result.x = (p1.x + p2.x + p3.x) / 3.0f;
  result.y = (p1.y + p2.y + p3.y) / 3.0f;
  result.z = (p1.z + p2.z + p3.z) / 3.0f;
}

void MeshGenerator::ScaleVector(XMFLOAT3& result, const float s)
{
  result.x *= s;
  result.y *= s;
  result.z *= s;
}

void MeshGenerator::CalculateUV(VertexTexture& vertex, const XMFLOAT4& min_coords, const XMFLOAT4& max_coords)
{
  XMFLOAT2 uvResult = {
    (vertex.position.x - min_coords.x) / (max_coords.x - min_coords.x),
    (vertex.position.y - min_coords.y) / (max_coords.y - min_coords.y)
  };

  vertex.uv = uvResult;
}

void MeshGenerator::CalculateUV_Cylindrical(VertexTexture& vertex, const XMFLOAT4& min_coords, const XMFLOAT4& max_coords)
{
  XMFLOAT2 uvResult = { 0, 0 };

  const XMFLOAT3 transMatVector = {
    (max_coords.x + min_coords.x) / 2.0f,
    min_coords.y,
    //(max_coords.y + min_coords.y) / 2.0f,
    (max_coords.z + min_coords.z) / 2.0f,
  };

  const XMFLOAT3 scaleMatVector = {
    2.0f / (max_coords.x - min_coords.x),
    1.0f / (max_coords.y - min_coords.y),
    2.0f / (max_coords.z - min_coords.z)
  };

  XMMATRIX scaleMatrix = DirectX::XMMatrixIdentity();
  scaleMatrix.r[0].m128_f32[0] = scaleMatVector.x;
  scaleMatrix.r[1].m128_f32[1] = scaleMatVector.y;
  scaleMatrix.r[2].m128_f32[2] = scaleMatVector.z;


  XMMATRIX translateMatrix = DirectX::XMMatrixIdentity();
  translateMatrix.r[0].m128_f32[3] = transMatVector.x;
  translateMatrix.r[1].m128_f32[3] = transMatVector.y;
  translateMatrix.r[2].m128_f32[3] = transMatVector.z;

  const XMVECTOR pos = { vertex.position.x, vertex.position.y, vertex.position.z, 1.0f };
  const XMVECTOR newVertexPos = DirectX::XMVector4Transform(pos, DirectX::XMMatrixMultiply(scaleMatrix, translateMatrix));

  float theta = std::atan2(newVertexPos.m128_f32[2], newVertexPos.m128_f32[0]);

  while (theta < 0)
    theta += (DirectX::XM_2PI);

  while (theta > DirectX::XM_2PI)
    theta -= (DirectX::XM_2PI);

  uvResult.x = theta / (DirectX::XM_2PI);
  uvResult.y = newVertexPos.m128_f32[1];

  //while (uvResult.x < 0)
  //  uvResult.x += 1.0f;

  //while (uvResult.x > 1.0f)
  //  uvResult.x -= 1.0f;

  //while (uvResult.y < 0)
  //  uvResult.y += 1.0f;

  //while (uvResult.y > 1.0f)
  //  uvResult.y -= 1.0f;

  vertex.uv_Cylindrical = uvResult;
}

void MeshGenerator::CalculateUV_Spherical(VertexTexture& vertex, const XMFLOAT4& min_coords, const XMFLOAT4& max_coords)
{
  XMFLOAT2 uvResult = { 0, 0 };

  const XMFLOAT3 transMatVector = {
    (max_coords.x + min_coords.x) / 2.0f,
    (max_coords.y + min_coords.y) / 2.0f,
    //min_coords.y,
    //(max_coords.y + min_coords.y) / 2.0f,
    (max_coords.z + min_coords.z) / 2.0f,
  };

  const XMFLOAT3 scaleMatVector = {
    1.0f / (max_coords.x - min_coords.x),
    1.0f / (max_coords.y - min_coords.y),
    1.0f / (max_coords.z - min_coords.z)
  };

  const XMMATRIX scaleMatrix = XMMatrixScaling(scaleMatVector.x, scaleMatVector.y, scaleMatVector.z);
  const XMMATRIX translateMatrix = DirectX::XMMatrixTranspose(XMMatrixTranslation(transMatVector.x, transMatVector.y, transMatVector.z));

  const XMVECTOR pos = { vertex.position.x, vertex.position.y, vertex.position.z, 1.0f };
  const XMVECTOR newVertexPos = DirectX::XMVector4Transform(pos, scaleMatrix * translateMatrix);

  float theta = std::atan2(newVertexPos.m128_f32[1], newVertexPos.m128_f32[0]);

  while (theta < 0)
    theta += (DirectX::XM_2PI);

  while (theta > DirectX::XM_2PI)
    theta -= (DirectX::XM_2PI);

  uvResult.x = theta / (DirectX::XM_2PI);

  const float r = sqrt((newVertexPos.m128_f32[0] * newVertexPos.m128_f32[0]) + 
    (newVertexPos.m128_f32[1] * newVertexPos.m128_f32[1]) + 
    (newVertexPos.m128_f32[2] * newVertexPos.m128_f32[2])
  );

  float phi = r == 0 ? 0 : acos(newVertexPos.m128_f32[2] / r);

  while (phi < 0)
    phi += DirectX::XM_PI;

  while (phi > DirectX::XM_PI)
    phi -= DirectX::XM_PI;

  uvResult.y = phi / DirectX::XM_PI;

  //while (uvResult.x < 0)
  //  uvResult.x += 1.0f;

  //while (uvResult.x > 1.0f)
  //  uvResult.x -= 1.0f;

  //while (uvResult.y < 0)
  //  uvResult.y += 1.0f;

  //while (uvResult.y > 1.0f)
  //  uvResult.y -= 1.0f;

  vertex.uv_Spherical = uvResult;
}

void MeshGenerator::CalculateTangentBinormal(MeshFace face, VertexTexture& vertex1, VertexTexture& vertex2, VertexTexture& vertex3,
  VecNormalMap& tangentMap, VecNormalMap& biTangentMap,
  VecNormalMap& tangentMap__Cylindrical, VecNormalMap& biTangentMap_Cylindrical,
  VecNormalMap& tangentMap_Spherical, VecNormalMap& biTangentMap_Spherical)
{

  //PLANAR
  std::pair<XMFLOAT3, XMFLOAT3> currentR = CalculateTangentBinormal_Helper(vertex1.position, vertex2.position, vertex3.position, vertex1.uv, vertex2.uv, vertex3.uv);
  tangentMap[face.v1_ID].push_back(currentR.first);
  tangentMap[face.v2_ID].push_back(currentR.first);
  tangentMap[face.v3_ID].push_back(currentR.first);

  biTangentMap[face.v1_ID].push_back(currentR.second);
  biTangentMap[face.v2_ID].push_back(currentR.second);
  biTangentMap[face.v3_ID].push_back(currentR.second);

  //CYLINDRICAL
  currentR = CalculateTangentBinormal_Helper(vertex1.position, vertex2.position, vertex3.position, vertex1.uv_Cylindrical, vertex2.uv_Cylindrical, vertex3.uv_Cylindrical);
  tangentMap__Cylindrical[face.v1_ID].push_back(currentR.first);
  tangentMap__Cylindrical[face.v2_ID].push_back(currentR.first);
  tangentMap__Cylindrical[face.v3_ID].push_back(currentR.first);

  biTangentMap_Cylindrical[face.v1_ID].push_back(currentR.second);
  biTangentMap_Cylindrical[face.v2_ID].push_back(currentR.second);
  biTangentMap_Cylindrical[face.v3_ID].push_back(currentR.second);

  //SPHERICAL
  currentR = CalculateTangentBinormal_Helper(vertex1.position, vertex2.position, vertex3.position, vertex1.uv_Spherical, vertex2.uv_Spherical, vertex3.uv_Spherical);
  tangentMap_Spherical[face.v1_ID].push_back(currentR.first);
  tangentMap_Spherical[face.v2_ID].push_back(currentR.first);
  tangentMap_Spherical[face.v3_ID].push_back(currentR.first);

  biTangentMap_Spherical[face.v1_ID].push_back(currentR.second);
  biTangentMap_Spherical[face.v2_ID].push_back(currentR.second);
  biTangentMap_Spherical[face.v3_ID].push_back(currentR.second);


  //////////////////////////////////////////////////////////////////////////
}

void MeshGenerator::EliminateDuplicateVector(std::vector<XMFLOAT3>& vecMap, XMFLOAT3& resultModify)
{
    //Remove duplicates if we have more than 1 normal
    //if (vecMap.size() > 1)
    //{
    //  const auto& last = std::unique(vecMap.begin(), vecMap.end(),
    //    [](const XMFLOAT3& v1, const XMFLOAT3& v2)
    //  {
    //    //lambda function that returns whether 2 XMFLOAT3 vectors are equal
    //    if (std::fabs(v1.x - v2.x) > 0.0001f || std::fabs(v1.y - v2.y) > 0.0001f || std::fabs(v1.z - v2.z) > 0.0001f)
    //    {
    //      return false;
    //    }
    //    return true;
    //  }/*XMF3*/
    //  );

    //  //remove the normals that are not needed
    //  vecMap.erase(last, vecMap.end());
    //}

    XMFLOAT3 tempResult(0, 0, 0);

    for (int j = 0; j < vecMap.size(); ++j)
    {
      tempResult.x += vecMap[j].x;
      tempResult.y += vecMap[j].y;
      tempResult.z += vecMap[j].z;
    }

    tempResult.x /= (float)vecMap.size();
    tempResult.y /= (float)vecMap.size();
    tempResult.z /= (float)vecMap.size();

    NormalizeFLOAT3(tempResult);

    resultModify = tempResult;
}


std::pair<XMFLOAT3, XMFLOAT3> MeshGenerator::CalculateTangentBinormal_Helper(const XMFLOAT3& v1_pos, const XMFLOAT3& v2_pos, const XMFLOAT3& v3_pos, const XMFLOAT2& v1_uv, const XMFLOAT2& v2_uv, const XMFLOAT2& v3_uv)
{
  XMFLOAT3 tangent, binormal;

  float vector1[3], vector2[3];
  float tuVector[2], tvVector[2];
  float den;
  float length;

  //////////////////////////////////////////////////////////////////////////
  //CALCULATE TANGENT & BI TANGENT
  // Calculate the two vectors for this face.
  vector1[0] = v2_pos.x - v1_pos.x;
  vector1[1] = v2_pos.y - v1_pos.y;
  vector1[2] = v2_pos.z - v1_pos.z;

  vector2[0] = v3_pos.x - v1_pos.x;
  vector2[1] = v3_pos.y - v1_pos.y;
  vector2[2] = v3_pos.z - v1_pos.z;

  // Calculate the tu and tv texture space vectors.
  tuVector[0] = v2_uv.x - v1_uv.x;
  tvVector[0] = v2_uv.y - v1_uv.y;

  tuVector[1] = v3_uv.x - v1_uv.x;
  tvVector[1] = v3_uv.y - v1_uv.y;

  // Calculate the denominator of the tangent/binormal equation.
  const float a = (tuVector[0] * tvVector[1] - tuVector[1] * tvVector[0]);
  den = 1.0f / (a == 0 ? 1.0f : a);

  // Calculate the cross products and multiply by the coefficient to get the tangent and binormal.
  tangent.x = (tvVector[1] * vector1[0] - tvVector[0] * vector2[0]) * den;
  tangent.y = (tvVector[1] * vector1[1] - tvVector[0] * vector2[1]) * den;
  tangent.z = (tvVector[1] * vector1[2] - tvVector[0] * vector2[2]) * den;

  binormal.x = (tuVector[0] * vector2[0] - tuVector[1] * vector1[0]) * den;
  binormal.y = (tuVector[0] * vector2[1] - tuVector[1] * vector1[1]) * den;
  binormal.z = (tuVector[0] * vector2[2] - tuVector[1] * vector1[2]) * den;

  // Calculate the length of this normal.
  length = sqrt((tangent.x * tangent.x) + (tangent.y * tangent.y) + (tangent.z * tangent.z));

  if (length)
  {
    // Normalize the normal and then store it
    tangent.x = tangent.x / length;
    tangent.y = tangent.y / length;
    tangent.z = tangent.z / length;
  }
  else
    tangent.x = tangent.y = tangent.z = 0;

  // Calculate the length of this normal.
  length = sqrt((binormal.x * binormal.x) + (binormal.y * binormal.y) + (binormal.z * binormal.z));

  if (length)
  {
    // Normalize the normal and then store it
    binormal.x = binormal.x / length;
    binormal.y = binormal.y / length;
    binormal.z = binormal.z / length;
  }
  else
    binormal.x = binormal.y = binormal.z = 0;

  return std::pair<XMFLOAT3, XMFLOAT3>(tangent, binormal);
}

void MeshGenerator::CalculateNormal(const XMFLOAT3& tangent, const XMFLOAT3& biTangent, XMFLOAT3& normal)
{
  // Calculate the cross product of the tangent and binormal which will give the normal vector.
  normal.x = (tangent.y * biTangent.z) - (tangent.z * biTangent.y);
  normal.y = (tangent.z * biTangent.x) - (tangent.x * biTangent.z);
  normal.z = (tangent.x * biTangent.y) - (tangent.y * biTangent.x);

  // Calculate the length of the normal.
  const float length = sqrt((normal.x * normal.x) + (normal.y * normal.y) + (normal.z * normal.z));

  // Normalize the normal.
  normal.x = normal.x / length;
  normal.y = normal.y / length;
  normal.z = normal.z / length;
}
