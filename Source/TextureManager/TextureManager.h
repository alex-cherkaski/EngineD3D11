#pragma once
#include "Macros.h"
#include "TextureData.h"

class TextureManager final
{
	SINGLETON(TextureManager);

public:
	const TextureData& CreateTextureData(const wchar_t* name, const wchar_t* path);
	bool HaveTextureData(const wchar_t* name);
	const TextureData& GetTextureDataRead(const wchar_t* name) const;
	void DeleteTextureData(const wchar_t* name);

	void Clear() { m_textureDataMap.clear(); }

private:
	std::unordered_map<const wchar_t*, TextureData> m_textureDataMap;
};

