#pragma once
#include "ShaderData.h"
#include "Macros.h"

class ShaderManager final
{
	SINGLETON(ShaderManager);

public:
	const ShaderData& CreateShaderData(const wchar_t* name, const wchar_t* vertexShaderPath, const wchar_t* pixelShaderPath);
	bool HaveShaderData(const wchar_t* name);
	const ShaderData& GetShaderDataRead(const wchar_t* name) const;
	void DeleteShaderData(const wchar_t* name);

	void Clear() { m_shaderDataMap.clear(); }

private:
	std::unordered_map<const wchar_t*, ShaderData> m_shaderDataMap;
};

