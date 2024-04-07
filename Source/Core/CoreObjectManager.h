#pragma once
#include "PCH.h"
#include "CoreStructs.h"
#include "Macros.h"

class CoreObjectManager final
{
	SINGLETON(CoreObjectManager);

public:
	void Initialize();

	void InitializeSprites();
	void InitializeUITexts();
	void Initialize3DModels();

	const std::vector<CoreObject>& GetCoreSpritesRead() const { return m_coreSprites; }
	std::vector<CoreObject>& GetCoreSpritesWrite() { return m_coreSprites; }

	const std::vector<CoreObject>& GetCoreUITextsRead() const { return m_coreUITexts; }
	std::vector<CoreObject>& GetCoreUITextsWrite() { return m_coreUITexts; }

	const std::vector<CoreObject>& GetCore3DModelsRead() const { return m_core3DModels; }
	std::vector<CoreObject>& GetCore3DModelsWrite() { return m_core3DModels; }

private:
	void SetUIText(GPUModelData& gpuModelData);

private:
	std::vector<CoreObject> m_coreSprites = { };
	std::vector<CoreObject> m_coreUITexts = { };
	std::vector<CoreObject> m_core3DModels = { };
};

