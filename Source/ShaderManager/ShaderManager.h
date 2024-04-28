#pragma once
#include "ShaderData.h"
#include "Macros.h"

class ShaderManager final
{
	SINGLETON(ShaderManager);

public:
	const ShaderData& CreateMeshShaderData(const std::wstring& name, const std::wstring& vertexShaderPath, const std::wstring& pixelShaderPath);
	const ShaderData& CreateUIShaderData(const std::wstring& name, const std::wstring& vertexShaderPath, const std::wstring& pixelShaderPath);
	bool HaveShaderData(const std::wstring& name);
	const ShaderData& GetShaderDataRead(const std::wstring& name) const;
	void DeleteShaderData(const std::wstring& name);

	void Clear() { m_shaderDataMap.clear(); }

private:
	std::unordered_map<std::wstring, ShaderData> m_shaderDataMap;
};

