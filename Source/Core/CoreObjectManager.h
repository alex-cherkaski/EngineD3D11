#pragma once
#include "PCH.h"
#include "CoreStructs.h"
#include "Macros.h"

class CoreObject final
{
public:
	CoreObject() = default;
	CoreObject(GUID guid) : m_gpuMeshDataGUID(guid) { }
public:
	GUID m_gpuMeshDataGUID = { 0 };
};

class CoreObjectManager final
{
	SINGLETON(CoreObjectManager);

public:
	void Initialize();
	const std::vector<CoreObject>& GetCoreObjectsRead() const { return m_coreObjects; }

private:
	std::vector<CoreObject> m_coreObjects;
};

