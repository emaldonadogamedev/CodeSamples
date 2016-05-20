
local assignmentTitle = "Meh Renderer"

if _ACTION == "clean" then
  os.rmdir("../bin")
  os.rmdir("./projects")
  os.rmdir("./ipch")
  os.remove("Meh_Renderer_1.sln")
  os.remove("Meh_Renderer_1.sdf")
  os.remove("Meh_Renderer_1.suo")
  os.remove("Meh_Renderer_1.v11.suo")
  os.remove("Meh_Renderer_1.v12.suo")
  os.exit()
end

solution "Meh_Renderer_1"
  configurations { "Debug", "ReleaseSymbols", "Release" }
  project(assignmentTitle)
    targetname(assignmentTitle:lower())
    --kind "ConsoleApp"
    kind "WindowedApp"
    language "C++"
    location "projects"
    --pchsource "../src/Precompiled.cpp"
    --pchheader "Precompiled.h"
    
    includedirs { 
      "../inc", 
      "../dep",
      "../dep/AntTweakBar/include",
      "../dep/ImGui/include"
      --"$(DXSDK_DIR)Include"
    }
    
    libdirs { 
      --"../dep/FreeGLUT", 
      --"../dep/GLEW", 
      "../dep/ImGui/lib", 
      "../dep/STB",
      "../dep/AntTweakBar/lib",
      "../dep/DirectXTK/Lib"
      --"$(DXSDK_DIR)Lib/x86"
      --"../dep/DX11/DLLs"
      }
    links { 
      --"freeglut", "glew32",
      "d3d11", "d3dcompiler", "AntTweakBar", "DirectXTK", "DirectXTex"--"d3dx11", "DxErr", -- DX11 Libs
    }
    
    files { "../inc/**.h", "../src/**.cpp" }
    configuration "Debug"
      targetdir "../bin/debug"
      defines { "_DEBUG", "ASSET_PATH=\"../../assets/\"" }
      flags { "Symbols", "WinMain" }
      
       
      links { 
      "ImGuid", -- debug version of ImGui 
      } 
      postbuildcommands {
        --"copy ..\\..\\dep\\GLEW\\glew32.dll ..\\..\\bin\\debug\\",
        --"copy ..\\..\\dep\\FreeGLUT\\freeglut.dll ..\\..\\bin\\debug\\",
        "copy ..\\..\\dep\\DX11\\DLLs\\d3dcompiler_47.dll ..\\..\\bin\\debug\\", -- DX11 DLLs
        "copy ..\\..\\dep\\\AntTweakBar\\lib\\AntTweakBar.dll ..\\..\\bin\\debug\\"
        }
        
    configuration "ReleaseSymbols"
      targetdir "../bin/release"
      defines { "NDEBUG", "ASSET_PATH=\"../../assets/\"" }
      flags { "Optimize", "Symbols", "WinMain" }
      links { "ImGui" } -- release version of ImGui
      linkoptions { "/LTCG" } -- allows whole-program optimization
      postbuildcommands {
        --"copy ..\\..\\dep\\GLEW\\glew32.dll ..\\..\\bin\\release\\",
        --"copy ..\\..\\dep\\FreeGLUT\\freeglut.dll ..\\..\\bin\\release\\",
        "copy ..\\..\\dep\\DX11\\DLLs\\d3dcompiler_47.dll ..\\..\\bin\\release\\", -- DX11 DLLs
        "copy ..\\..\\dep\\\AntTweakBar\\lib\\AntTweakBar.dll ..\\..\\bin\\release\\"
        }
    
    configuration "Release"
      targetdir "../bin/release"
      defines { "NDEBUG", "ASSET_PATH=\"../../assets/\"" }
      flags { "Optimize", "WinMain" }
      links { "ImGuid" } -- release version of ImGui
      linkoptions { "/LTCG" } -- allows whole-program optimization
      postbuildcommands {
        --"copy ..\\..\\dep\\GLEW\\glew32.dll ..\\..\\bin\\release\\",
        --"copy ..\\..\\dep\\FreeGLUT\\freeglut.dll ..\\..\\bin\\release\\",
        "copy ..\\..\\dep\\DX11\\DLLs\\d3dcompiler_47.dll ..\\..\\bin\\release\\", -- DX11 DLLs
        "copy ..\\..\\dep\\\AntTweakBar\\lib\\AntTweakBar.dll ..\\..\\bin\\release\\"
        }
