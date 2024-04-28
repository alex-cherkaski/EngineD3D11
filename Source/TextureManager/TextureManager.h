#pragma once
#include "Macros.h"
#include "TextureData.h"

class TextureManager final
{
	SINGLETON(TextureManager);

public:
	const TextureData& CreateTextureData(const std::wstring& name, const std::wstring& path);
	bool HaveTextureData(const std::wstring& name) const;
	const TextureData& GetTextureDataRead(const std::wstring& name) const;
	void DeleteTextureData(const std::wstring& name);

	void Clear() { m_textureDataMap.clear(); }
 
private:
	std::unordered_map<std::wstring, TextureData> m_textureDataMap;
};

