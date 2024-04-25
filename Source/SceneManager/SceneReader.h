#pragma once
#include "PCH.h"

struct SceneReader
{
	Microsoft::WRL::ComPtr<IStream> XMLFileStream = nullptr;
	Microsoft::WRL::ComPtr<IXmlReader> XMLReader = nullptr;
};

