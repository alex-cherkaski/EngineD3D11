#pragma once
#include "CoreStructs.h"
#include "PCH.h"
#include "Macros.h"

class CoreGPUDataManager final
{
	SINGLETON(CoreGPUDataManager);

public:
	GUID GenerateGUID() const;

	const GPUModelData& GetGPUModelDataRead(GUID guid);
	GPUModelData& GetGPUModelDataWrite(GUID guid);

private:
	std::unordered_map<unsigned short, GPUModelData> m_gpuModelDataMap;
};

