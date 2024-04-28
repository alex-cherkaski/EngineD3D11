#pragma once
#include "Macros.h"
#include "MeshManager/MeshData.h"
#include "TextureManager/TextureData.h"
#include "UIData.h"

struct CharacterTextureBox;

class UIManager final
{
	SINGLETON(UIManager);

	struct CharacterTextureBox
	{
		float x = 0;
		float y = 0;
		float w = 0;
		float h = 0;
	};

public:
	const UIData& CreateUIData(const std::wstring& name, const std::wstring& text);
	const UIData& ReCreateUIData(const std::wstring& name, const std::wstring& text);
	bool HaveUIData(const std::wstring& name);
	const UIData& GetUIDataRead(const std::wstring& name) const;
	void DeleteUIData(const std::wstring& name);

	void Clear() { m_uiDataMap.clear(); }

private:
	void ConstructUIMesh(UIData& uiData, const std::wstring& text);

	void ConstructCharacterPlane(UIData& uiData, wchar_t character, byte index);
	CharacterTextureBox ConstructCharacterBox(wchar_t character);

private:
	std::unordered_map<std::wstring, UIData> m_uiDataMap;
};

