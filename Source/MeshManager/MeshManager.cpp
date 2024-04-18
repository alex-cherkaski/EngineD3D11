#include "PCH.h"
#include "MeshManager.h"
#include "Core/Core.h"

void MeshManager::Initialize()
{
	//CreateMesh("Space Ship", "./Resources/Models/SciFi_Fighter_AK5.obj");
	const auto a = CreateMesh(L"Cube", L"./Resources/Models/capsule.obj");

	int breakpoint = 0;
}

const Mesh& MeshManager::CreateMesh(const wchar_t* name, const wchar_t* path)
{
	ENGINE_ASSERT_W(m_meshMap.find(name) == m_meshMap.cend(), "Mesh with name %s already exists.", name);
	m_meshMap[name].Load(path);
	ENGINE_ASSERT_W(!m_meshMap[name].vertices.empty(), "Failed to load mesh %s from %s.", name, path);
	return m_meshMap[name];
}

const Mesh& MeshManager::GetMesh(const wchar_t* name) const
{
	const auto constIterator = m_meshMap.find(name);
	ENGINE_ASSERT_W(constIterator != m_meshMap.cend(), "Failed to find mesh %s.", name);
	return constIterator->second;
}

