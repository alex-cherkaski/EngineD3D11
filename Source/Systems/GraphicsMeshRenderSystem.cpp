#include "PCH.h"
#include "Components/Components.h"
#include "ECS/Registry.h"
#include "GraphicsMeshRenderSystem.h"
#include "Cameras/FirstPersonCamera.h"
#include "Core/Core.h"
#include "MeshManager/MeshManager.h"
#include "ShaderManager/ShaderManager.h"
#include "TextureManager/TextureManager.h"

GraphicsMeshRenderSystem::GraphicsMeshRenderSystem(Registry& registry)
	: ISystem::ISystem(registry)
{
	RequireComponent<TransformComponent>();
	RequireComponent<GraphicsMeshComponent>();
}

void GraphicsMeshRenderSystem::Render()
{
	// Retrieve the camera view and projection matrices required for rendering.
	const FirstPersonCamera& firstPersonCamera = FirstPersonCamera::GetInstanceRead();
	const XMMATRIX viewMatrix = firstPersonCamera.GetViewMatrix();
	const XMMATRIX projectionMatrix = firstPersonCamera.GetPerspectiveMatrix();

	// Retrieve the renderer to update and render; and relevent managers to query for relevant data.
	Renderer& renderer = Renderer::GetInstanceWrite();
	const MeshManager& meshManager = MeshManager::GetInstanceRead();
	const ShaderManager& shaderManager = ShaderManager::GetInstanceRead();
	const TextureManager& textureManager = TextureManager::GetInstanceRead();

	for (const Entity entity : m_entities)
	{
		// Retrieve the relevant components.
		const TransformComponent& transformComponent = m_registry.GetComponentRead<TransformComponent>(entity);
		const GraphicsMeshComponent& graphicsMeshComponent = m_registry.GetComponentRead<GraphicsMeshComponent>(entity);

		// Retrieve the relevant graphics data.
		const MeshData& meshData = meshManager.GetMeshDataRead(graphicsMeshComponent.MeshName);
		const ShaderData& shaderData = shaderManager.GetShaderDataRead(graphicsMeshComponent.ShaderName);

		// Update the GPU constant buffer with the entity model/world matrix.
		renderer.UpdatePerMeshConstantBuffer(transformComponent.Transform);

		// Disable any color blending when drawing a regular 3D mesh.
		renderer.DisableBlending();

		// If the mesh has a texture, draw it with the specified texture.
		if (graphicsMeshComponent.TextureName)
		{
			// Retrieve the texture data referencing the texture the mesh should be drawn with.
			const TextureData& textureData = textureManager.GetTextureDataRead(graphicsMeshComponent.TextureName);

			// Draw the mesh using the relevant mesh data.
			renderer.DrawMesh(&meshData, &shaderData, &textureData);
		}

		// Otherwise draw the mesh without a texture.
		else
		{
			renderer.DrawMesh(&meshData, &shaderData);
		}
	}
}

