#include "PCH.h"
#include "UIManager.h"
#include "Core/Core.h"

const UIData& UIManager::CreateUIData(const std::wstring& name, const std::wstring& text)
{
	// Ensure that an entry with the same name does not already exit.
	const auto constIterator = m_uiDataMap.find(name);
	ENGINE_ASSERT_W(constIterator == m_uiDataMap.cend(), "Entry for text %s already exists.", name.c_str());

	// Create a text data entry and construct its corresponding mesh.
	UIData& uiData = m_uiDataMap[name];
	ConstructUIMesh(uiData, text);

	// Return the newly filled text data struct.
	return uiData;
}

const UIData& UIManager::ReCreateUIData(const std::wstring& name, const std::wstring& text)
{
	// Erase any previous entries for this text.
	m_uiDataMap.erase(name);

	// Create a text data entry and construct its corresponding mesh.
	UIData& uiData = m_uiDataMap[name];
	ConstructUIMesh(uiData, text);

	// Return the newly filled text data struct.
	return uiData;
}

bool UIManager::HaveUIData(const std::wstring& name)
{
	// Attempt to find an entry with a corresponding key.
	return m_uiDataMap.find(name) != m_uiDataMap.cend();
}

const UIData& UIManager::GetUIDataRead(const std::wstring& name) const
{
	// Attempt to find a text data entry with the corresponding name.
	const auto constIterator = m_uiDataMap.find(name);
	ENGINE_ASSERT_W(constIterator != m_uiDataMap.cend(), "Entry for text %s not found.", name.c_str());
	return constIterator->second;
}

void UIManager::DeleteUIData(const std::wstring& name)
{
	// Attempt to find and erase a text data entry with the corresponding name.
	const auto constIterator = m_uiDataMap.find(name);
	ENGINE_ASSERT_W(constIterator != m_uiDataMap.cend(), "Entry for text %s not found.", name.c_str());
	m_uiDataMap.erase(name);
}

void UIManager::ConstructUIMesh(UIData& uiData, const std::wstring& text)
{
	ENGINE_ASSERT_W(text.size() <= 32, "Text character count too large."); // Arbitrary choice for testing.

	for (byte i = 0; i < text.size(); ++i)
	{
		ConstructCharacterPlane(uiData, text[i], i);
	}

	uiData.Mesh.PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	Renderer& renderer = Renderer::GetInstanceWrite();
	renderer.CreateDynamicVertexBuffer(uiData);
}

void UIManager::ConstructCharacterPlane(UIData& uiData, wchar_t character, byte index)
{
	constexpr float CHAR_TEXTURE_WIDTH = 10;
	constexpr float CHAR_TEXTURE_HEIGHT = 20;

	const CharacterTextureBox box = ConstructCharacterBox(character);
	
	UIVertexAttributes vertexAttribute1;
	vertexAttribute1.Position.x = index * CHAR_TEXTURE_WIDTH;
	vertexAttribute1.Position.y = 0.0f;
	vertexAttribute1.Position.z = 0.0f;
	vertexAttribute1.Texture.x = box.x;
	vertexAttribute1.Texture.y = box.y;
	
	UIVertexAttributes vertexAttribute2;
	vertexAttribute2.Position.x = index * CHAR_TEXTURE_WIDTH + CHAR_TEXTURE_WIDTH;
	vertexAttribute2.Position.y = 0.0f;
	vertexAttribute2.Position.z = 0.0f;
	vertexAttribute2.Texture.x = box.x + box.w;
	vertexAttribute2.Texture.y = box.y;

	UIVertexAttributes vertexAttribute3;
	vertexAttribute3.Position.x = index * CHAR_TEXTURE_WIDTH + CHAR_TEXTURE_WIDTH;
	vertexAttribute3.Position.y = CHAR_TEXTURE_HEIGHT;
	vertexAttribute3.Position.z = 0.0f;
	vertexAttribute3.Texture.x = box.x + box.w;
	vertexAttribute3.Texture.y = box.y + box.h;

	UIVertexAttributes vertexAttribute4;
	vertexAttribute4.Position.x = index * CHAR_TEXTURE_WIDTH + CHAR_TEXTURE_WIDTH;
	vertexAttribute4.Position.y = CHAR_TEXTURE_HEIGHT;
	vertexAttribute4.Position.z = 0.0f;
	vertexAttribute4.Texture.x = box.x + box.w;
	vertexAttribute4.Texture.y = box.y + box.h;

	UIVertexAttributes vertexAttribute5;
	vertexAttribute5.Position.x = index * CHAR_TEXTURE_WIDTH;
	vertexAttribute5.Position.y = CHAR_TEXTURE_HEIGHT;
	vertexAttribute5.Position.z = 0.0f;
	vertexAttribute5.Texture.x = box.x;
	vertexAttribute5.Texture.y = box.y + box.h;

	UIVertexAttributes vertexAttribute6;
	vertexAttribute6.Position.x = index * CHAR_TEXTURE_WIDTH;
	vertexAttribute6.Position.y = 0.0f;
	vertexAttribute6.Position.z = 0.0f;
	vertexAttribute6.Texture.x = box.x;
	vertexAttribute6.Texture.y = box.y;

	uiData.Mesh.Vertices.push_back(vertexAttribute1);
	uiData.Mesh.Vertices.push_back(vertexAttribute2);
	uiData.Mesh.Vertices.push_back(vertexAttribute3);
	uiData.Mesh.Vertices.push_back(vertexAttribute4);
	uiData.Mesh.Vertices.push_back(vertexAttribute5);
	uiData.Mesh.Vertices.push_back(vertexAttribute6);

	int breakpoint = 0;
}

UIManager::CharacterTextureBox UIManager::ConstructCharacterBox(wchar_t character)
{
	// Attempt to extract the ascii character from the wide character.
	char asciiChar = 0;
#pragma warning(disable : 4996)
	const size_t count = wcstombs(&asciiChar, &character, 1);
#pragma warning(default : 4996)
	ENGINE_ASSERT_W(count > 0, "Failed to convert wide character %c to ascii string.", character);
	ENGINE_ASSERT_W(asciiChar >= 32 && asciiChar <= 126, "Text character %c out of bounds.", character);

	// The dimensions of a single character on the texture atlas.
	constexpr float CHAR_TEXTURE_WIDTH = 10.0f;
	constexpr float CHAR_TEXTURE_HEIGHT = 20.0f;

	// The dimensions of the entire texture atlas.
	constexpr float TEXTURE_WIDTH = 950.0f;
	constexpr float TEXTURE_HEIGHT = 20.0f;

	// The normalized width and height of a single character.
	constexpr float NORMALIZED_CHARACTER_WIDTH = CHAR_TEXTURE_WIDTH / TEXTURE_WIDTH;
	constexpr float NORMALIZED_CHARACTER_HEIGHT = CHAR_TEXTURE_HEIGHT / TEXTURE_HEIGHT;

	// Compute the character box dimensions on the character texture atlas.
	CharacterTextureBox box = { 0 };
	box.x = ((asciiChar - 32) * CHAR_TEXTURE_WIDTH) / TEXTURE_WIDTH;
	box.y = 0.0f;
	box.w = NORMALIZED_CHARACTER_WIDTH;
	box.h = NORMALIZED_CHARACTER_HEIGHT;

	return box;
}
