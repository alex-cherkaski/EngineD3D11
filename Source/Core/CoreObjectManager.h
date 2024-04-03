#pragma once
#include "PCH.h"
#include "CoreStructs.h"
#include "Macros.h"



class CoreObjectManager final
{
	SINGLETON(CoreObjectManager);

public:
	void Initialize();
	const std::vector<CoreObject>& GetCoreObjectsRead() const { return m_coreObjects; }
	std::vector<CoreObject>& GetCoreObjectsWrite() { return m_coreObjects; }

private:
	std::vector<CoreObject> m_coreObjects;
};

