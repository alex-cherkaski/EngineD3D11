#include "PCH.h"
#include "Core/Core.h"
#include "TextureManager.h"

const TextureData& TextureManager::CreateTextureData(const std::wstring& name, const std::wstring& path)
{
	// Ensure there are no previous entries for a texture with the same name stored in the texture data map.
	ENGINE_ASSERT(!HaveTextureData(name), "Already have an entry for texture %s from %s.", name.c_str(), path.c_str());

	// Create a new instance of the texture data struct inside the texture data map.
	TextureData& textureData = m_textureDataMap[name];

	// Fill out the texture file path and .dds statues
	textureData.TextureFilePath = path;
	textureData.IsDDS = StrStr(path.c_str(), L".dds") != nullptr;

	// Upload the texture to the GPU and create a shader resource view interface for it.
	Renderer& renderer = Renderer::GetInstanceWrite();
	renderer.CreateShaderResourceViewFromFile(textureData);

	return textureData;
}

bool TextureManager::HaveTextureData(const std::wstring& name) const
{
	// Attempt to search for an entry with a matching name key.
	return m_textureDataMap.find(name) != m_textureDataMap.cend();
}

const TextureData& TextureManager::GetTextureDataRead(const std::wstring& name) const
{
	// Attempt to search for an entry with a matching name key.
	const auto constIterator = m_textureDataMap.find(name);
	ENGINE_ASSERT(constIterator != m_textureDataMap.cend(), "Do not have an entry for texture %s.", name.c_str());
	return constIterator->second;
}

void TextureManager::DeleteTextureData(const std::wstring& name)
{
	// Attempt to search for an entry with a matching name key.
	const auto constIterator = m_textureDataMap.find(name);
	ENGINE_ASSERT(constIterator != m_textureDataMap.cend(), "Do not have an entry for texture %s.", name.c_str());
	m_textureDataMap.erase(constIterator);
}
