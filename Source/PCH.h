#pragma once

#if _WIN32 || _WIN64
#if _WIN64
#define ENVIRONMENT64
#else
#define ENVIRONMENT32
#endif
#endif

#include <Windows.h>
#include <wrl.h>	// ComPtr
#include <comdef.h> // COM error reporting.
#include <rpcdce.h> // For UUID generation.
#include <rpcnterr.h> // For UUID error check.

#include <cassert>
#include <cstdio>
#include <cstdlib>

#include <unordered_map>
#include <string>
#include <vector>

#include <d3d11.h>
#include <dxgi.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
//#include <DirectXColors.h>

using namespace DirectX;

#include "Includes/DXUT/dxerr.h"

#include "Includes/DirectXTK/WICTextureLoader.h"
#include "Includes/DirectXTK/DDSTextureLoader.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
//#pragma comment(lib, "winmm.lib")
#pragma  comment(lib, "comsuppw.lib") // COM error reporting.

#pragma comment(lib, "DXUT.lib")
#pragma comment(lib, "DXUTOpt.lib")
#pragma comment(lib, "comctl32.lib") // Needed for one of the functions in DXUT. https://forums.codeguru.com/showthread.php?43971-error-LNK2001-unresolved-external-symbol-__imp__InitCommonControls-0

#pragma comment(lib, "DirectXTK.lib")

#pragma comment(lib, "Rpcrt4.lib") // For UUID generation.

