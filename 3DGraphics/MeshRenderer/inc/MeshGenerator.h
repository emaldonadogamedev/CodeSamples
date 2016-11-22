#pragma once

#include <unordered_map>
#include "VertexTypes.h"

#define BOX_MESH "box"
#define SPHERE_MESH "sphere"
#define QUAD "quad"
#define DEBUG_FACE_NORMAL "_Face_Normals"
#define DEBUG_VERTEX_NORMAL "_Vertex_Normals"


class VertexBuffer;
class IndexBuffer;

struct MeshBuffer
{
  MeshBuffer() :vBuffer(nullptr), iBuffer(nullptr), index_count(0), vertex_count(0), m_centroid(0,0,0){}

  XMFLOAT3 m_centroid;
  VertexBuffer *vBuffer;
  IndexBuffer *iBuffer;
  unsigned int vertex_count;
  unsigned int index_count;
};

typedef std::vector< std::vector<XMFLOAT3> > VecNormalMap;

class MeshGenerator
{
public:
  //Getters
  static VertexBuffer* getObjVertexBuffer(std::string objName);
  static VertexBuffer* getObjVertexVNormalBuffer(std::string objName);
  static VertexBuffer* getObjVertexFaceNormalBuffer(std::string objName);

  static IndexBuffer* getObjIndexBuffer(std::string objName);
  static MeshBuffer* getObjMeshBuffer(std::string objName);
  static MeshData* getObjMeshDataPtr(std::string objName);

  //Basic Shapes
  static MeshBuffer CreateBox(MeshData& meshData, float width = 1.0f, float height = 1.0f, float depth = 1.0f, XMFLOAT4 color = XMFLOAT4(1, 1, 1, 1));
  static MeshBuffer CreateQuad(MeshData& meshData, float width = 1.0f, float height = 1.0f, XMFLOAT4 color = XMFLOAT4(1, 1, 1, 1));
  static MeshBuffer CreateSphere(MeshData& meshData, float radius, unsigned int sliceCount, unsigned int stackCount);

  //File loading functions
  static MeshBuffer& CS300LoadMeshFromOBJ(MeshData& meshData, std::string const &objFile);
   static bool isXMFLOAT3Equal(const XMFLOAT3& p1, const XMFLOAT3& p2);
  
   //Release
  static void ReleaseMeshBuffers();
  static VertexBuffer* m_currBindedVertexBuffer;
  static IndexBuffer* m_currBindedIndexBuffer;

private:
  //Math helper functions
  static void CrossProduct(XMFLOAT3& v1, XMFLOAT3& v2, XMFLOAT3& result);
  static void SubtractVec(XMFLOAT3& v1, XMFLOAT3& v2, XMFLOAT3& result);
  static void NormalizeFLOAT3(XMFLOAT3& v);
  static void FindTriangleCentroid(const XMFLOAT3& p1, const XMFLOAT3& p2, const XMFLOAT3& p3, XMFLOAT3& result);
  static void ScaleVector(XMFLOAT3& result, const float s);

  //texture coordinate functions
  static void CalculateUV(VertexTexture& vertex, const XMFLOAT4& min_coords, const XMFLOAT4& max_coords);
  static void CalculateUV_Cylindrical(VertexTexture& vertex, const XMFLOAT4& min_coords, const XMFLOAT4& max_coords);
  static void CalculateUV_Spherical(VertexTexture& vertex, const XMFLOAT4& min_coords, const XMFLOAT4& max_coords);

  //Tangent BiTangent Helper function
  static void CalculateTangentBinormal(MeshFace face, VertexTexture& vertex1, VertexTexture& vertex2, VertexTexture& vertex3, 
    VecNormalMap& tangentMap, VecNormalMap& biTangentMap,
    VecNormalMap& tangentMap__Cylindrical, VecNormalMap& biTangentMap_Cylindrical,
    VecNormalMap& tangentMap_Spherical, VecNormalMap& biTangentMap_Spherical);
  
  static std::pair<XMFLOAT3, XMFLOAT3> CalculateTangentBinormal_Helper(const XMFLOAT3& v1_pos, const XMFLOAT3& v2_pos, 
    const XMFLOAT3& v3_pos, const XMFLOAT2& v1_uv, const XMFLOAT2& v2_uv, const XMFLOAT2& v3_uv);
  static void EliminateDuplicateVector(std::vector<XMFLOAT3>&, XMFLOAT3& resultModify);

  static void CalculateNormal(const XMFLOAT3& tangent, const XMFLOAT3& biTangent, XMFLOAT3& normal );

  //static properties
  static const std::string m_meshAssetsDir;
  static std::unordered_map<std::string, MeshData> m_meshCollection;
  static std::unordered_map<std::string, MeshBuffer> m_meshBufferCollection;
};