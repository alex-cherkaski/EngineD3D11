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
}

void SceneManager::Update(float deltaTime)
{
	static Registry& registry = Registry::GetInstanceWrite();
	registry.RunSystemsUpdate(deltaTime);
}

void SceneManager::Render()
{
	static Registry& registry = Registry::GetInstanceWrite();
	registry.RunSystemsRender();
}

void SceneManager::CreateSceneData()
{
	MeshManager& meshManager = MeshManager::GetInstanceWrite();
	meshManager.CreateMeshData(
		L"Spot_Mesh",
		L"./Resources/Meshes/spot_triangulated.obj"
	);

	ShaderManager& shaderManager = ShaderManager::GetInstanceWrite();
	shaderManager.CreateShaderData(
		L"LightingShader",
		L"./Source/Shaders/LightingShader.hlsl",
		L"./Source/Shaders/LightingShader.hlsl"
	);

	TextureManager& textureManager = TextureManager::GetInstanceWrite();
	textureManager.CreateTextureData(
		L"Spot_Texture",
		L"./Resources/Textures/spot_texture.png"
	);
}

void SceneManager::CreateEntities()
{
	Registry& registry = Registry::GetInstanceWrite();

	Entity Spot = registry.CreateEntity();
	{
		TransformComponent transformComponent;
		transformComponent.Transform._43 = 6.0f;

		PhysicsComponent physicsComponent;

		GraphicsMeshComponent graphicsMeshComponent;
		graphicsMeshComponent.MeshName = L"Spot_Mesh";
		graphicsMeshComponent.ShaderName = L"LightingShader";
		graphicsMeshComponent.TextureName = L"Spot_Texture";

		registry.AddComponent<TransformComponent>(Spot, transformComponent);
		registry.AddComponent<PhysicsComponent>(Spot, physicsComponent);
		registry.AddComponent<GraphicsMeshComponent>(Spot, graphicsMeshComponent);
	}
}

void SceneManager::CreateSystems()
{
	Registry& registry = Registry::GetInstanceWrite();
	registry.AddSystem<PhysicsSystem>();
	registry.AddSystem<GraphicsMeshRenderSystem>();
}
