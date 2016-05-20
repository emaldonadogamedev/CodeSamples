#pragma once

//Components & System
#include"IComponent.h"
#include"IRendererComp.h"

#include "ISystem.h"

#include"GameTimer.h"
#include "MathHelper.h"

//DirectX Includes
#include<d3d11.h>
#include<DirectXColors.h>
#include"dxerr.h"
#include<DirectXMath.h>
#include<DirectXTK/Inc/DDSTextureLoader.h>
#include<DirectXTK/Inc/DirectXTex.h>

//STL Includes
#include<vector>
#include<map>
#include<unordered_map>
#include<memory>
#include<algorithm>

//Assertion
#include <assert.h>

//time
#include<time.h>

//HRESULT CHECK
#ifndef HR
#ifdef _DEBUG
#define  HR(x) \
            		{ \
			HRESULT hr = x; \
			if(FAILED(hr)) \
            			{\
				DXTraceW(__FILEW__, __LINE__, hr, L#x, TRUE); \
            			}\
            		}
#else  // !_DEBUG
#define  HR(x) x;
#endif  // _DEBUG
#endif // !HR

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")
//#pragma comment(lib, "msvcprtd.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "comctl32.lib")
//#pragma comment(lib, "dxerr.lib")
//  

//#include <>

//#pragma comment(lib, "AntTweakBar.lib")

namespace Memory
{
  template<typename T> void SafeDelete(T& t)
  {
    if (t)
    {
      delete t;
      t = 0;
    }
  }

  template<typename T> void SafeDeleteArr(T& t)
  {
    if (t)
    {
      delete [] t;
      t = 0;
    }
  }

  template<typename T> void SafeRelease(T& t)
  {
    if (t)
    {
      t->Release();
      t = 0;
    }
  }
}
