#include "PCH.h"
#include "Core/Core.h"
#include "ShaderManager.h"

const ShaderData& ShaderManager::CreateMeshShaderData(const std::wstring& name, const std::wstring& vertexShaderPath, const std::wstring& pixelShaderPath)
{
	// Ensure there are no previous entries for a shader with the same name stored in the shader data map.
	ENGINE_ASSERT(!HaveShaderData(name), "Already have an entry for shader %s.", name.c_str());

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

const ShaderData& ShaderManager::CreateUIShaderData(const std::wstring& name, const std::wstring& vertexShaderPath, const std::wstring& pixelShaderPath)
{
	// Ensure there are no previous entries for a shader with the same name stored in the shader data map.
	ENGINE_ASSERT(!HaveShaderData(name), "Already have an entry for shader %s.", name.c_str());

	// Create a new instance of the shader data struct inside the shader data map.
	ShaderData& shaderData = m_shaderDataMap[name];

	// Fill the relevant shader data required to create the actual shader interfaces.
	shaderData.VertexShaderPath = vertexShaderPath;
	shaderData.PixelShaderPath = pixelShaderPath;

	// Create the shader relevant vertex and pixel shader interfaces; as well as the input layout.
	Renderer& renderer = Renderer::GetInstanceWrite();
	renderer.CreateVertexShader(shaderData);
	renderer.CreatePixelShader(shaderData);
	renderer.CreateUIInputLayout(shaderData);

	return shaderData;
}

bool ShaderManager::HaveShaderData(const std::wstring& name)
{
	// Attempt to search for an entry with a matching name key.
	return m_shaderDataMap.find(name) != m_shaderDataMap.cend();
}

const ShaderData& ShaderManager::GetShaderDataRead(const std::wstring& name) const
{
	// Attempt to search for an entry with a matching name key, and if found return it.
	const auto constIterator = m_shaderDataMap.find(name.c_str());
	ENGINE_ASSERT(constIterator != m_shaderDataMap.cend(), "Do not have an entry for shader %s.", name.c_str());
	return constIterator->second;
}

void ShaderManager::DeleteShaderData(const std::wstring& name)
{
	// Attempt to search for an entry with a matching name key, and if found erase it.
	const auto constIterator = m_shaderDataMap.find(name.c_str());
	ENGINE_ASSERT(constIterator != m_shaderDataMap.cend(), "Do not have an entry for shader %s.", name.c_str());
	m_shaderDataMap.erase(constIterator);
}
