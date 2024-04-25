#include "PCH.h"
#include "ECS/Registry.h"
#include "Components/Components.h"
#include "MeshManager/MeshManager.h"
#include "SceneManager.h"
#include "ShaderManager/ShaderManager.h"
#include "Systems/PhysicsSystem.h"
#include "Systems/GraphicsMeshRenderSystem.h"
#include "TextureManager/TextureManager.h"

void SceneManager::Initialize()
{
	CreateSceneData();
	CreateEntities();
	CreateSystems();
	LoadScene(L"./Resources/Scenes/Viking_Room.xml");
}

void SceneManager::Update(float deltaTime)
{
	static Registry& registry = Registry::GetInstanceWrite();
	registry.RunSystemsUpdate(deltaTime);

	m_currentScene->Update(deltaTime);
}

void SceneManager::Render()
{
	static Registry& registry = Registry::GetInstanceWrite();
	registry.RunSystemsRender();

	m_currentScene->Render();
}

//void SceneManager::DeleteXMLFileReader(const wchar_t* name)
//{
//	// Attempt to find and erase the file reader.
//	const auto constIterator = m_sceneReaderMap.find(name);
//	ENGINE_ASSERT_W(m_sceneReaderMap.find(name) == m_sceneReaderMap.cend(), "Scene %s already exitst", name);
//	m_sceneReaderMap.erase(constIterator);
//}

void SceneManager::LoadScene(const wchar_t* filePath)
{
	m_currentScene = std::make_unique<Scene>(filePath);
}

void SceneManager::CreateSceneData()
{
	constexpr bool IS_OPENGL_MESH = true;

	MeshManager& meshManager = MeshManager::GetInstanceWrite();
	ShaderManager& shaderManager = ShaderManager::GetInstanceWrite();
	TextureManager& textureManager = TextureManager::GetInstanceWrite();

	shaderManager.CreateShaderData(
		L"LightingShader",
		L"./Source/Shaders/LightingShader.hlsl",
		L"./Source/Shaders/LightingShader.hlsl"
	);

	// Viking.
	{
		meshManager.CreateMeshData(
			L"Viking_Mesh",
			L"./Resources/Meshes/viking_room.obj",
			IS_OPENGL_MESH
		);

		textureManager.CreateTextureData(
			L"Viking_Texture",
			L"./Resources/Textures/viking_room.png"
		);
	}

	//// Spot.
	//{
	//	meshManager.CreateMeshData(
	//		L"Spot_Mesh",
	//		L"./Resources/Meshes/spot_triangulated.obj",
	//		IS_OPENGL_MESH
	//	);

	//	textureManager.CreateTextureData(
	//		L"Spot_Texture",
	//		L"./Resources/Textures/spot_texture.png"
	//	);
	//}

	//// Bob.
	//{
	//	meshManager.CreateMeshData(
	//		L"Bob_Mesh",
	//		L"./Resources/Meshes/bob_tri.obj",
	//		IS_OPENGL_MESH
	//	);

	//	textureManager.CreateTextureData(
	//		L"Bob_Texture",
	//		L"./Resources/Textures/bob_diffuse.png"
	//	);
	//}

	//// Blub.
	//{
	//	meshManager.CreateMeshData(
	//		L"Blub_Mesh",
	//		L"./Resources/Meshes/blub_triangulated.obj",
	//		IS_OPENGL_MESH
	//	);

	//	textureManager.CreateTextureData(
	//		L"Blub_Texture",
	//		L"./Resources/Textures/blub_texture.png"
	//	);
	//}

	//// Ogre.
	//{
	//	meshManager.CreateMeshData(
	//		L"Ogre_Mesh",
	//		L"./Resources/Meshes/bs_rest.obj",
	//		IS_OPENGL_MESH
	//	);

	//	textureManager.CreateTextureData(
	//		L"Ogre_Texture",
	//		L"./Resources/Textures/ogre_diffuse.png"
	//	);
	//}
}

void SceneManager::CreateEntities()
{
	Registry& registry = Registry::GetInstanceWrite();

	//Entity Spot = registry.CreateEntity();
	//{
	//	TransformComponent transformComponent;
	//	AdjustMeshTranslation(transformComponent, -2.0f, 6.0f, 11.0f);
	//	AdjustMeshRotation(transformComponent, -90.0f, -180.0f, 0.0f);

	//	PhysicsComponent physicsComponent;

	//	GraphicsMeshComponent graphicsMeshComponent;
	//	graphicsMeshComponent.MeshName = L"Spot_Mesh";
	//	graphicsMeshComponent.ShaderName = L"LightingShader";
	//	graphicsMeshComponent.TextureName = L"Spot_Texture";

	//	registry.AddComponent<TransformComponent>(Spot, transformComponent);
	//	registry.AddComponent<PhysicsComponent>(Spot, physicsComponent);
	//	registry.AddComponent<GraphicsMeshComponent>(Spot, graphicsMeshComponent);
	//}

	//Entity Bob = registry.CreateEntity();
	//{
	//	TransformComponent transformComponent;
	//	AdjustMeshTranslation(transformComponent, 6.0f, 5.5f, 11.0f);
	//	AdjustMeshRotation(transformComponent, -90.0f, -90.0f, 0.0f);

	//	PhysicsComponent physicsComponent;

	//	GraphicsMeshComponent graphicsMeshComponent;
	//	graphicsMeshComponent.MeshName = L"Bob_Mesh";
	//	graphicsMeshComponent.ShaderName = L"LightingShader";
	//	graphicsMeshComponent.TextureName = L"Bob_Texture";

	//	registry.AddComponent<TransformComponent>(Bob, transformComponent);
	//	registry.AddComponent<PhysicsComponent>(Bob, physicsComponent);
	//	registry.AddComponent<GraphicsMeshComponent>(Bob, graphicsMeshComponent);
	//}

	//Entity Blub = registry.CreateEntity();
	//{
	//	TransformComponent transformComponent;
	//	AdjustMeshTranslation(transformComponent, 0.0f, 5.5f, 5.5f);
	//	AdjustMeshRotation(transformComponent, -90.0f, 0.0f, 0.0f);

	//	PhysicsComponent physicsComponent;

	//	GraphicsMeshComponent graphicsMeshComponent;
	//	graphicsMeshComponent.MeshName = L"Blub_Mesh";
	//	graphicsMeshComponent.ShaderName = L"LightingShader";
	//	graphicsMeshComponent.TextureName = L"Blub_Texture";

	//	registry.AddComponent<TransformComponent>(Blub, transformComponent);
	//	registry.AddComponent<PhysicsComponent>(Blub, physicsComponent);
	//	registry.AddComponent<GraphicsMeshComponent>(Blub, graphicsMeshComponent);
	//}

	//Entity Ogre = registry.CreateEntity();
	//{
	//	TransformComponent transformComponent;
	//	AdjustMeshTranslation(transformComponent, 0.0f, 1.5f, 7.0f);
	//	AdjustMeshRotation(transformComponent, -90.0f, 0.0f, 0.0f);

	//	PhysicsComponent physicsComponent;

	//	GraphicsMeshComponent graphicsMeshComponent;
	//	graphicsMeshComponent.MeshName = L"Ogre_Mesh";
	//	graphicsMeshComponent.ShaderName = L"LightingShader";
	//	graphicsMeshComponent.TextureName = L"Ogre_Texture";

	//	registry.AddComponent<TransformComponent>(Ogre, transformComponent);
	//	registry.AddComponent<PhysicsComponent>(Ogre, physicsComponent);
	//	registry.AddComponent<GraphicsMeshComponent>(Ogre, graphicsMeshComponent);
	//}

	Entity viking_room = registry.CreateEntity();
	{
		TransformComponent transformComponent;
		AdjustMeshTranslation(transformComponent, 0.0f, 0.0f, 5.0f);
		AdjustMeshRotation(transformComponent, 0.0f, 90.0f, 0.0f);

		PhysicsComponent physicsComponent;

		GraphicsMeshComponent graphicsMeshComponent;
		graphicsMeshComponent.MeshName = L"Viking_Mesh";
		graphicsMeshComponent.ShaderName = L"LightingShader";
		graphicsMeshComponent.TextureName = L"Viking_Texture";

		registry.AddComponent<TransformComponent>(viking_room, transformComponent);
		registry.AddComponent<PhysicsComponent>(viking_room, physicsComponent);
		registry.AddComponent<GraphicsMeshComponent>(viking_room, graphicsMeshComponent);
	}
}

void SceneManager::CreateSystems()
{
	Registry& registry = Registry::GetInstanceWrite();
	registry.AddSystem<PhysicsSystem>();
	registry.AddSystem<GraphicsMeshRenderSystem>();
}

void SceneManager::AdjustMeshTranslation(TransformComponent& transformComponent, float x, float y, float z)
{
	// Apply the translation to the transform of the entity.
	XMMATRIX transform = XMLoadFloat4x4(&transformComponent.Transform);
	transform *= XMMatrixTranslation(x, y, z);
	XMStoreFloat4x4(&transformComponent.Transform, transform);
}

void SceneManager::AdjustMeshRotation(TransformComponent& transformComponent, float x, float y, float z)
{
	XMMATRIX transform = XMLoadFloat4x4(&transformComponent.Transform);

	const XMMATRIX localSpace = XMMatrixTranslation(
		-transformComponent.Transform._41,
		-transformComponent.Transform._42,
		-transformComponent.Transform._43
	);

	const XMMATRIX rotation = XMMatrixRotationRollPitchYaw(
		XMConvertToRadians(x),
		XMConvertToRadians(y),
		XMConvertToRadians(z)
	);

	const XMMATRIX worldSpace = XMMatrixTranslation(
		transformComponent.Transform._41,
		transformComponent.Transform._42,
		transformComponent.Transform._43
	);

	// Translate the entity to the origin, perform the rotation, and translate the entity back to world space.
	transform *= localSpace * rotation * worldSpace;

	XMStoreFloat4x4(&transformComponent.Transform, transform);
}
