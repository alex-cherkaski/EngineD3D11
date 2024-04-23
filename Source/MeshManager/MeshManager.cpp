#include "PCH.h"
#include "MeshManager.h"
#include "Core/Core.h"

//void MeshManager::Initialize()
//{
//	//CreateMesh("Space Ship", "./Resources/Models/SciFi_Fighter_AK5.obj");
//	//const auto a = CreateMesh(L"Cube", L"./Resources/Models/capsule.obj");
//	const auto b = CreateMesh(L"Spot", L"./Resources/Models/spot_triangulated.obj");
//	//const auto c = CreateMesh(L"Ogre", L"./Resources/Models/bs_rest.obj");
//
//	int breakpoint = 0;
//}

const MeshData& MeshManager::CreateMeshData(const wchar_t* name, const wchar_t* path)
{
	// Ensure there are no previous entries for a mesh with the same name stored in the mesh data map.
	ENGINE_ASSERT_W(m_meshDataMap.find(name) == m_meshDataMap.cend(), "Mesh with name %s already exists.", name);

	// Load the mesh and get an instance of the mesh data struct from the map.
	Mesh mesh = LoadMesh(path);
	MeshData& meshData = m_meshDataMap[name];
	
	// Copy over the mesh vertex attributes.
	meshData.Vertices.resize(mesh.vertices.size());
	for (size_t i = 0; i < mesh.vertices.size(); ++i)
	{
		meshData.Vertices[i].Position = mesh.vertices[i].position;
		meshData.Vertices[i].Normal = mesh.vertices[i].normal;
		meshData.Vertices[i].Texture = mesh.vertices[i].textureCoordinate;
	}

	// Copy over the mesh index data.
	meshData.Indices.resize(mesh.indices.size());
	for (size_t i = 0; i < mesh.indices.size(); ++i)
	{
		meshData.Indices[i] = mesh.indices[i];
	}

	// Create the GPU side vertex and index buffers, and set the primitive topology enum.
	Renderer& renderer = Renderer::GetInstanceWrite();
	renderer.CreateDefaultVertexBuffer(meshData);
	renderer.CreateIndexBuffer(meshData);
	meshData.PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	return meshData;
}

bool MeshManager::HaveMeshData(const wchar_t* name)
{
	// Attempt to search for an entry with a matching name key.
	return m_meshDataMap.find(name) != m_meshDataMap.cend();
}

const MeshData& MeshManager::GetMeshDataRead(const wchar_t* name) const
{
	// Attempt to search for an entry with a matching name key, and if found return it.
	const auto constIterator = m_meshDataMap.find(name);
	ENGINE_ASSERT_W(constIterator != m_meshDataMap.cend(), "Do not have an entry for mesh %s.", name);
	return constIterator->second;
}

void MeshManager::DeleteMeshData(const wchar_t* name)
{
	// Attempt to search for an entry with a matching name key, and if found erase it.
	const auto constIterator = m_meshDataMap.find(name);
	ENGINE_ASSERT_W(constIterator != m_meshDataMap.cend(), "Do not have an entry for mesh %s.", name);
	m_meshDataMap.erase(constIterator);
}

const Mesh MeshManager::LoadMesh(const wchar_t* path)
{
	Mesh mesh;
	mesh.Load(path, false);
	ENGINE_ASSERT_W(!mesh.vertices.empty(), "Failed to load mesh from %s.", path);

	// Convert the GL mesh UVs to D3D mesh UVs.
	for (auto& vertex : mesh.vertices)
	{
		//vertex.textureCoordinate.x *= -1;
		vertex.textureCoordinate.y *= -1;
		std::swap(vertex.position.y, vertex.position.z);
	}

	if (!mesh.hasNormals)
	{
		GenerateMeshNormals(mesh);
	}
	
	return mesh;
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

