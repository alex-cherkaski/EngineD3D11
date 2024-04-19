#pragma once
#include "Macros.h"

using Mesh = WaveFrontReader<UINT>;

class MeshManager final
{
	SINGLETON(MeshManager);

public:
	void Initialize();

	const Mesh& CreateMesh(const wchar_t* name, const wchar_t* path);
	const Mesh& GetMesh(const wchar_t* name) const;

	void DeleteMesh(const wchar_t* name) { m_meshMap.erase(name); }
	void Clear() { m_meshMap.clear(); }

private:
	void GenerateMeshNormals(Mesh& mesh);

private:
	std::unordered_map<const wchar_t*, Mesh> m_meshMap;
};

