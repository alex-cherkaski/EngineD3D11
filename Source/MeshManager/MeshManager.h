#pragma once
#include "Macros.h"
#include "MeshData.h"

using Mesh = WaveFrontReader<UINT>;

class MeshManager final
{
	SINGLETON(MeshManager);

public:
	const MeshData& CreateMeshData(const wchar_t* name, const wchar_t* path);
	bool HaveMeshData(const wchar_t* name);
	const MeshData& GetMeshDataRead(const wchar_t* name) const;
	void DeleteMeshData(const wchar_t* name);

	void Clear() { m_meshDataMap.clear(); }

private:
	const Mesh LoadMesh(const wchar_t* path);
	void GenerateMeshNormals(Mesh& mesh);

private:
	std::unordered_map<const wchar_t*, MeshData> m_meshDataMap;
};

