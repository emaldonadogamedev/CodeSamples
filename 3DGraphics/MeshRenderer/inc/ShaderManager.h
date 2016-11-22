#pragma once

#include<unordered_map>
#include "VertexShader.h"
#include "PixelShader.h"

class ShaderManager
{
public:

  //Getters
  static Shader* getShader(std::string shader_name);
  static Shader* m_defaultVertexShader;
  static Shader* m_defaultPixelShader;

  //Setters
  static void addShader(Shader* shader);
  static void addShader(ShaderType shader_type, std::string shader_name);
  static void reAddShader(std::string shader_name);

  static void removeShader(Shader* shader);
  static void removeShader(std::string shader_name);

  static void clearShaders();

  static VertexShader* m_currBindedVertexShader;
  static PixelShader* m_currBindedPixelShader;

private:
  static std::unordered_map<std::string, Shader*> m_shaderCollection;
};