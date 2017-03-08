#pragma once

#if defined(DEBUG) || defined(_DEBUG)

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#endif

#include <windows.h>
#include <windowsx.h>

#include <cassert>
#include <ctime>
#include <algorithm>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <list>
#include "Engine/Common/MathHelper.h"
#include "Engine/Common/LightHelper.h"

//#include <d3d11.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
//#include <dxerr.h>
//#include <d3d11.h>
//#include <d3dx11.h>

#include "Engine/Common/Effect/d3dx11effect.h"

#include <DirectXTex.h>
//#include <D3DCompiler.h>
//#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <DirectXPackedVector.h>







using namespace DirectX;
using namespace DirectX::PackedVector;


#define WM_IMPORT_TEXTURE WM_USER + 1   
#define WM_SAVETEXTURE WM_USER + 2  
#define WM_IMPORT_MODEL WM_USER + 3  
#define WM_IMPORT_GEOMETRY WM_USER + 4  
#define  WM_CREATE_MATERIAL WM_USER + 5
//---------------------------------------------------------------------------------------
// Simple d3d error checker for book demos.
//---------------------------------------------------------------------------------------

#if defined(DEBUG) | defined(_DEBUG)
#ifndef HR
#define HR(x)									\
	{									\
		HRESULT hr = (x);						\
		if(FAILED(hr))							\
		{								\
			LPWSTR output;                                    	\
			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM |		\
				FORMAT_MESSAGE_IGNORE_INSERTS 	 |		\
				FORMAT_MESSAGE_ALLOCATE_BUFFER,			\
				NULL,						\
				hr,						\
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),	\
				(LPTSTR) &output,				\
				0,						\
				NULL);					        \
			MessageBox(NULL, output, L"Error", MB_OK);		\
		}								\
	}
#endif
#else
#ifndef HR
#define HR(x) (x)
#endif
#endif

#ifndef D3DX11INLINE
	#ifdef _MSC_VER
		#if (_MSC_VER >= 1200)
			#define D3DX11INLINE __forceinline
		#else
			#define D3DX11INLINE __inline
		#endif
	#else
		#ifdef __cplusplus
			#define D3DX11INLINE inline
		#else
			#define D3DX11INLINE
		#endif
	#endif
#endif


//---------------------------------------------------------------------------------------
// Convenience macro for releasing COM objects.
//---------------------------------------------------------------------------------------

#define ReleaseCOM(x) { if(x){ x->Release(); x = 0; } }

//---------------------------------------------------------------------------------------
// Convenience macro for deleting objects.
//---------------------------------------------------------------------------------------

#define SafeDelete(x) { delete x; x = 0; }

#define MAX_MATERIAL      10
#define MAX_TEXTURE       10


///////////////////////////////////////////////////////////////////////
static char * ConvertWCtoC(wchar_t* str)
{
	char* pStr;
	int strSize = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
	pStr = new char[strSize];
	WideCharToMultiByte(CP_ACP, 0, str, -1, pStr, strSize, 0, 0);
	return pStr;
}

///////////////////////////////////////////////////////////////////////
static wchar_t* ConverCtoWC(char* str)
{
	wchar_t* pStr;
	int strSize = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, NULL);
	pStr = new WCHAR[strSize];
	MultiByteToWideChar(CP_ACP, 0, str, strlen(str) + 1, pStr, strSize);
	return pStr;
}

enum RENDERTEXTURE
{
	SCENETEXTURE = 0, SILHOUETTE, REFLECTION
};

#define SHADOWMAP_RESOLUTION 1024


