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
	constexpr bool IS_OPENGL_MESH = true;

	MeshManager& meshManager = MeshManager::GetInstanceWrite();
	ShaderManager& shaderManager = ShaderManager::GetInstanceWrite();
	TextureManager& textureManager = TextureManager::GetInstanceWrite();

	shaderManager.CreateShaderData(
		L"LightingShader",
		L"./Source/Shaders/LightingShader.hlsl",
		L"./Source/Shaders/LightingShader.hlsl"
	);

	// Spot.
	{
		meshManager.CreateMeshData(
			L"Spot_Mesh",
			L"./Resources/Meshes/spot_triangulated.obj",
			IS_OPENGL_MESH
		);

		textureManager.CreateTextureData(
			L"Spot_Texture",
			L"./Resources/Textures/spot_texture.png"
		);
	}

	// Bob.
	{
		meshManager.CreateMeshData(
			L"Bob_Mesh",
			L"./Resources/Meshes/bob_tri.obj",
			IS_OPENGL_MESH
		);

		textureManager.CreateTextureData(
			L"Bob_Texture",
			L"./Resources/Textures/bob_diffuse.png"
		);
	}

	// Blub.
	{
		meshManager.CreateMeshData(
			L"Blub_Mesh",
			L"./Resources/Meshes/blub_triangulated.obj",
			IS_OPENGL_MESH
		);

		textureManager.CreateTextureData(
			L"Blub_Texture",
			L"./Resources/Textures/blub_texture.png"
		);
	}

	// Ogre.
	{
		meshManager.CreateMeshData(
			L"Ogre_Mesh",
			L"./Resources/Meshes/bs_rest.obj",
			IS_OPENGL_MESH
		);

		textureManager.CreateTextureData(
			L"Ogre_Texture",
			L"./Resources/Textures/ogre_diffuse.png"
		);
	}
}

void SceneManager::CreateEntities()
{
	Registry& registry = Registry::GetInstanceWrite();

	Entity Spot = registry.CreateEntity();
	{
		TransformComponent transformComponent;
		AdjustMeshTranslation(transformComponent, -2.0f, 6.0f, 11.0f);
		AdjustMeshRotation(transformComponent, -90.0f, -180.0f, 0.0f);

		PhysicsComponent physicsComponent;

		GraphicsMeshComponent graphicsMeshComponent;
		graphicsMeshComponent.MeshName = L"Spot_Mesh";
		graphicsMeshComponent.ShaderName = L"LightingShader";
		graphicsMeshComponent.TextureName = L"Spot_Texture";

		registry.AddComponent<TransformComponent>(Spot, transformComponent);
		registry.AddComponent<PhysicsComponent>(Spot, physicsComponent);
		registry.AddComponent<GraphicsMeshComponent>(Spot, graphicsMeshComponent);
	}

	Entity Bob = registry.CreateEntity();
	{
		TransformComponent transformComponent;
		AdjustMeshTranslation(transformComponent, 6.0f, 5.5f, 10.0f);
		AdjustMeshRotation(transformComponent, -90.0f, -90.0f, 0.0f);

		PhysicsComponent physicsComponent;

		GraphicsMeshComponent graphicsMeshComponent;
		graphicsMeshComponent.MeshName = L"Bob_Mesh";
		graphicsMeshComponent.ShaderName = L"LightingShader";
		graphicsMeshComponent.TextureName = L"Bob_Texture";

		registry.AddComponent<TransformComponent>(Bob, transformComponent);
		registry.AddComponent<PhysicsComponent>(Bob, physicsComponent);
		registry.AddComponent<GraphicsMeshComponent>(Bob, graphicsMeshComponent);
	}

	Entity Blub = registry.CreateEntity();
	{
		TransformComponent transformComponent;
		AdjustMeshTranslation(transformComponent, 0.0f, 5.5f, 5.5f);
		AdjustMeshRotation(transformComponent, -90.0f, 0.0f, 0.0f);

		PhysicsComponent physicsComponent;

		GraphicsMeshComponent graphicsMeshComponent;
		graphicsMeshComponent.MeshName = L"Blub_Mesh";
		graphicsMeshComponent.ShaderName = L"LightingShader";
		graphicsMeshComponent.TextureName = L"Blub_Texture";

		registry.AddComponent<TransformComponent>(Blub, transformComponent);
		registry.AddComponent<PhysicsComponent>(Blub, physicsComponent);
		registry.AddComponent<GraphicsMeshComponent>(Blub, graphicsMeshComponent);
	}

	Entity Ogre = registry.CreateEntity();
	{
		TransformComponent transformComponent;
		AdjustMeshTranslation(transformComponent, 0.0f, 1.5f, 6.5f);
		AdjustMeshRotation(transformComponent, -90.0f, 0.0f, 0.0f);

		PhysicsComponent physicsComponent;

		GraphicsMeshComponent graphicsMeshComponent;
		graphicsMeshComponent.MeshName = L"Ogre_Mesh";
		graphicsMeshComponent.ShaderName = L"LightingShader";
		graphicsMeshComponent.TextureName = L"Ogre_Texture";

		registry.AddComponent<TransformComponent>(Ogre, transformComponent);
		registry.AddComponent<PhysicsComponent>(Ogre, physicsComponent);
		registry.AddComponent<GraphicsMeshComponent>(Ogre, graphicsMeshComponent);
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
	transform += XMMatrixTranslation(x, y, z);
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

	// Translate the entity to the origin, perform the rotation, andtranslate the entity back to world space.
	transform *= localSpace * rotation * worldSpace;

	XMStoreFloat4x4(&transformComponent.Transform, transform);
}
