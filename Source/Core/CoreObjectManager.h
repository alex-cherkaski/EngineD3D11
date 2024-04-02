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

	const std::vector<CoreObject>& GetSpritesRead() const { return m_sprites; }
	std::vector<CoreObject>& GetSpritesWrite() { return m_sprites; }

	const std::vector<CoreObject>& GetUITextsRead() const { return m_uiTexts; }
	std::vector<CoreObject>& GetUITextsWrite() { return m_uiTexts; }

private:
	std::vector<CoreObject> m_sprites = { };
	std::vector<CoreObject> m_uiTexts = { };
};

