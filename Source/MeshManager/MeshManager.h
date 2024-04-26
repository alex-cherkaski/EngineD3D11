#pragma once
#include "Macros.h"
#include "MeshData.h"

using Mesh = WaveFrontReader<UINT>;

class MeshManager final
{
	SINGLETON(MeshManager);

public:
	const MeshData& CreateMeshData(const std::wstring& name, const std::wstring& path, bool isOpenGLMesh = false);
	bool HaveMeshData(const std::wstring& name);
	const MeshData& GetMeshDataRead(const std::wstring& name) const;
	void DeleteMeshData(const std::wstring& name);

	void Clear() { m_meshDataMap.clear(); }

private:
	std::unique_ptr<Mesh> LoadMesh(const std::wstring& path, bool isOpenGLMesh = false);
	void GenerateMeshNormals(Mesh& mesh);

private:
	std::unordered_map<std::wstring, MeshData> m_meshDataMap;
};

