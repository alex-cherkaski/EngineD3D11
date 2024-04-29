#include "PCH.h"
#include "Components/Components.h"
#include "Core/Core.h"
#include "ECS/Registry.h"
#include "ShaderManager/ShaderManager.h"
#include "TextureManager/TextureManager.h"
#include "UIManager/UIManager.h"
#include "UIRenderSystem.h"

UIRenderSystem::UIRenderSystem(Registry& registry)
	: ISystem::ISystem(registry)
{
	RequireComponent<TransformComponent>();
	RequireComponent<UIComponent>();
}

void UIRenderSystem::Update(float deltaTime)
{
	// Retrieve the relevant modules to update the average fps element.
	static const Engine& engine = Engine::GetInstanceRead();
	static UIManager& uiManager = UIManager::GetInstanceWrite();
	static Entity avgFPSEntity = *m_entities.cbegin(); // For testing purposes assume its the first.
	static const UIComponent& uiComponent = m_registry.GetComponentRead<UIComponent>(avgFPSEntity);

	// Recreate the ui element mesh based on the new average fps value.
	const std::wstring averageFPS = L"Average FPS: " + std::to_wstring(engine.GetAverageFPS());
	uiManager.ReCreateUIData(uiComponent.UIName, averageFPS);
}

void UIRenderSystem::Render()
{
	// Retrieve the relevant manager to query for render data, and renderer to do the rendering.
	static const ShaderManager& shaderManager = ShaderManager::GetInstanceRead();
	static const TextureManager& textureManager = TextureManager::GetInstanceRead();
	static UIManager& uiManager = UIManager::GetInstanceWrite();
	static Renderer& renderer = Renderer::GetInstanceWrite();

	for (const Entity entity : m_entities)
	{
		// Retrieve the relevant components to query for data relevant for rendering.
		const TransformComponent& transformComponent = m_registry.GetComponentRead<TransformComponent>(entity);
		const UIComponent& uiComponent = m_registry.GetComponentRead<UIComponent>(entity);

		// Retrieve the relevant shader, texture, and ui data to render the ui element.
		const ShaderData& shaderData = shaderManager.GetShaderDataRead(uiComponent.ShaderName);
		const TextureData& textureData = textureManager.GetTextureDataRead(uiComponent.TextureName);
		const UIData& uiData = uiManager.GetUIDataRead(uiComponent.UIName);
			
		// Update the ui element mesh vertex data.
		renderer.UpdateUITextVertexBuffer(uiData);

		// Update the world matrix constant buffer.
		renderer.UpdatePerMeshConstantBuffer(transformComponent.Transform);

		// Enable texture blending to blend the ui background with the rest of the color buffer contents.
		renderer.EnableBlending();

		// Draw the ui element.
		renderer.DrawUI(&uiData.Mesh, &shaderData, &textureData);
	}
}
