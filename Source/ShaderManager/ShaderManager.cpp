#include "PCH.h"
#include "Core/Core.h"
#include "ShaderManager.h"

const ShaderData& ShaderManager::CreateShaderData(const wchar_t* name, const wchar_t* vertexShaderPath, const wchar_t* pixelShaderPath)
{
	// Ensure there are no previous entries for a shader with the same name stored in the shader data map.
	ENGINE_ASSERT_W(!HaveShaderData(name), "Already have an entry for shader %s.", name);

	// Create a new instance of the shader data struct inside the shader data map.
	ShaderData& shaderData = m_shaderDataMap[name];

	// Fill the relevant shader data required to create the actual shader interfaces.
	shaderData.VertexShaderPath = vertexShaderPath;
	shaderData.PixelShaderPath = pixelShaderPath;

	// Create the shader relevant vertex and pixel shader interfaces; as well as the input layout.
	Renderer& renderer = Renderer::GetInstanceWrite();
	renderer.CreateVertexShader(shaderData);
	renderer.CreatePixelShader(shaderData);
	renderer.CreateInputLayout(shaderData);

	return shaderData;
}

bool ShaderManager::HaveShaderData(const wchar_t* name)
{
	// Attempt to search for an entry with a matching name key.
	return m_shaderDataMap.find(name) != m_shaderDataMap.cend();
}

const ShaderData& ShaderManager::GetShaderDataRead(const wchar_t* name) const
{
	// Attempt to search for an entry with a matching name key, and if found return it.
	const auto constIterator = m_shaderDataMap.find(name);
	ENGINE_ASSERT_W(constIterator != m_shaderDataMap.cend(), "Do not have an entry for shader %s.", name);
	return constIterator->second;
}

void ShaderManager::DeleteShaderData(const wchar_t* name)
{
	// Attempt to search for an entry with a matching name key, and if found erase it.
	const auto constIterator = m_shaderDataMap.find(name);
	ENGINE_ASSERT_W(constIterator != m_shaderDataMap.cend(), "Do not have an entry for shader %s.", name);
	m_shaderDataMap.erase(constIterator);
}
