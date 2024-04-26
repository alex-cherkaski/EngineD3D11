#pragma once

#if _WIN32 || _WIN64
#if _WIN64
#define ENVIRONMENT64
#else
#define ENVIRONMENT32
#endif
#endif

#include <Windows.h>
#include <wrl.h>		// ComPtr
#include <comdef.h>		// COM error reporting.
#include <rpcdce.h>		// For UUID generation.
#include <rpcnterr.h>	// For UUID error check.
#include <shlwapi.h>	// For StrStr function.
#include <xmllite.h>	// For XML reader.

#include <cassert>
#include <cstdio>
#include <cstdlib>

#include <bitset>
#include <queue>
#include <unordered_map>
#include <set>
#include <stack>
#include <string>
#include <vector>

#include <d3d11.h>
#include <dxgi.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
//#include <DirectXColors.h>

using namespace DirectX;

#include "Includes/DirectXTK/WICTextureLoader.h"
#include "Includes/DirectXTK/DDSTextureLoader.h"
#include "Includes/DirectXTK/Keyboard.h"
#include "Includes/DirectXTK/Mouse.h"
#include "Includes/DirectXTK/GamePad.h"

#include "Includes/DirectXMesh/Utilities/WaveFrontReader.h"
#include "Includes/DirectXMesh/DirectXMesh.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
//#pragma comment(lib, "winmm.lib")
#pragma  comment(lib, "comsuppw.lib") // COM error reporting.

#pragma comment(lib, "DirectXTK.lib")
#pragma comment(lib, "DirectXMesh.lib")

#pragma comment(lib, "Rpcrt4.lib")	// For UUID generation.
#pragma comment(lib, "Shlwapi.lib") // For StrStr function.
#pragma comment(lib, "XmlLite.lib")	// For XML reader.
