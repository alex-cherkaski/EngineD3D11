#include "PCH.h"
#include "CoreGPUDataManager.h"

GUID CoreGPUDataManager::GenerateGUID() const
{
	// Attempt to generate a GUID.
	GUID result = { 0 };
	const RPC_STATUS createUUIDStatus = UuidCreateSequential(&result);

	// Error check GUID generation and return the result.
	ENGINE_ASSERT(createUUIDStatus == RPC_S_OK, "Failed to generated adequate UUID.");
	return result;
}

const GPUModelData& CoreGPUDataManager::GetGPUModelDataRead(GUID guid)
{
	return GetGPUModelDataWrite(guid);
}

GPUModelData& CoreGPUDataManager::GetGPUModelDataWrite(GUID guid)
{
	// Attempt to generate a hash for the given GUID.
	RPC_STATUS guidHashStatus = 0;
	const unsigned short guidHash = UuidHash(&guid, &guidHashStatus);

	// Error check hash generation and retrieve the pointer.
	ENGINE_ASSERT(guidHashStatus == RPC_S_OK, "Failed to generate adequate hash for GUID.");
	return m_gpuModelDataMap[guidHash];
}

