#include "PCH.h"
#include "MeshManager.h"
#include "Core/Core.h"

void MeshManager::Initialize()
{
	//CreateMesh("Space Ship", "./Resources/Models/SciFi_Fighter_AK5.obj");
	//const auto a = CreateMesh(L"Cube", L"./Resources/Models/capsule.obj");
	const auto b = CreateMesh(L"Spot", L"./Resources/Models/spot_triangulated.obj");
	//const auto c = CreateMesh(L"Ogre", L"./Resources/Models/bs_rest.obj");

	int breakpoint = 0;
}

const Mesh& MeshManager::CreateMesh(const wchar_t* name, const wchar_t* path)
{
	ENGINE_ASSERT_W(m_meshMap.find(name) == m_meshMap.cend(), "Mesh with name %s already exists.", name);
	
	Mesh& newMesh = m_meshMap[name];
	newMesh.Load(path, false);
	ENGINE_ASSERT_W(!newMesh.vertices.empty(), "Failed to load mesh %s from %s.", name, path);

	// Convert the GL mesh UVs to D3D mesh UVs.
	for (auto& vertex : newMesh.vertices)
	{
		//vertex.textureCoordinate.x *= -1;
		vertex.textureCoordinate.y *= -1;
		std::swap(vertex.position.y, vertex.position.z);
	}

	if (!newMesh.hasNormals)
	{
		GenerateMeshNormals(newMesh);
	}

	
	return newMesh;
}

const Mesh& MeshManager::GetMesh(const wchar_t* name) const
{
	const auto constIterator = m_meshMap.find(name);
	ENGINE_ASSERT_W(constIterator != m_meshMap.cend(), "Failed to find mesh %s.", name);
	return constIterator->second;
}

void MeshManager::GenerateMeshNormals(Mesh& mesh)
{
	// Calculate the mesh face count and mesh vertex count.
	const size_t faceCount = mesh.indices.size() / 3;
	const size_t vertexCount = mesh.vertices.size();

	// Allocate a temporary deep copy of the mesh vertex positions array.
	std::unique_ptr<XMFLOAT3[]> positions = std::make_unique<XMFLOAT3[]>(vertexCount);
	for (size_t i = 0; i < vertexCount; ++i)
		positions[i] = mesh.vertices[i].position;

	// Attempt to generate the mesh normals.
	std::unique_ptr<XMFLOAT3[]> normals = std::make_unique<XMFLOAT3[]>(vertexCount);
	constexpr CNORM_FLAGS flags = CNORM_FLAGS::CNORM_DEFAULT;
	const HRESULT computeNormalsResult = ComputeNormals(
		mesh.indices.data(),		// Mesh vertex-index array.
		faceCount,					// The number of faces in the mesh.
		positions.get(),			// Array of mesh vertices.
		vertexCount,				// The number of mesh vertices.
		flags,						// Additional normal computation settings.
		normals.get()				// Output array of normals.
	);

	// Error check mesh normal generation.
	ENGINE_ASSERT_HRESULT(computeNormalsResult);

	// Copy over generated normals to the mesh.
	for (size_t i = 0; i < vertexCount; ++i)
		mesh.vertices[i].normal = normals[i];

	// Free unique pointers on scope exit...
}

