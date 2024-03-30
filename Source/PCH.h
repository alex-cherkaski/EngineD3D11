#pragma once

#include <Windows.h>
#include <wrl.h>	// ComPtr
#include <comdef.h> // COM error reporting.

#include <cassert>
#include <cstdio>
#include <cstdlib>

#include <string>
#include <vector>

#include <d3d11.h>
#include <dxgi.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
//#include <DirectXColors.h>

using namespace DirectX;

#include "Includes/DXUT/dxerr.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
//#pragma comment(lib, "winmm.lib")
#pragma  comment(lib, "comsuppw.lib") // COM error reporting.

#pragma comment(lib, "DXUT.lib")
#pragma comment(lib, "DXUTOpt.lib")
#pragma comment(lib, "comctl32.lib") // Needed for one of the functions in DXUT.

#pragma comment(lib, "DirectXTK.lib")

