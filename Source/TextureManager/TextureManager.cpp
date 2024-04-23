#include "PCH.h"
#include "Core/Core.h"
#include "TextureManager.h"

const TextureData& TextureManager::CreateTextureData(const wchar_t* name, const wchar_t* path)
{
	// Ensure there are no previous entries for a texture with the same name stored in the texture data map.
	ENGINE_ASSERT_W(!HaveTextureData(name), "Already have an entry for texture %s from %s.", name, path);

	// Create a new instance of the texture data struct inside the texture data map.
	TextureData& textureData = m_textureDataMap[name];

	// Fill out the texture file path and .dds statues
	textureData.TextureFilePath = path;
	textureData.IsDDS = StrStr(path, L".dds") != nullptr;

	// Upload the texture to the GPU and create a shader resource view interface for it.
	Renderer& renderer = Renderer::GetInstanceWrite();
	renderer.CreateShaderResourceViewFromFile(textureData);

	return textureData;
}

bool TextureManager::HaveTextureData(const wchar_t* name)
{
	// Attempt to search for an entry with a matching name key.
	return m_textureDataMap.find(name) != m_textureDataMap.cend();
}

const TextureData& TextureManager::GetTextureDataRead(const wchar_t* name) const
{
	// Attempt to search for an entry with a matching name key.
	const auto constIterator = m_textureDataMap.find(name);
	ENGINE_ASSERT_W(constIterator != m_textureDataMap.cend(), "Do not have an entry for texture %s.", name);
	return constIterator->second;
}

void TextureManager::DeleteTextureData(const wchar_t* name)
{
	// Attempt to search for an entry with a matching name key.
	const auto constIterator = m_textureDataMap.find(name);
	ENGINE_ASSERT_W(constIterator != m_textureDataMap.cend(), "Do not have an entry for texture %s.", name);
	m_textureDataMap.erase(constIterator);
}
