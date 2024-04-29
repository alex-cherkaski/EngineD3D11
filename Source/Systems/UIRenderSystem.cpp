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

UIRenderSystem::~UIRenderSystem()
{
}

void UIRenderSystem::Update(float deltaTime)
{
	//static UIManager& uiManager = UIManager::GetInstanceWrite();
	//const UIComponent& uiComponent = m_registry.GetComponentRead<UIComponent>(entity);

	//const std::wstring averageFPS = L"Average FPS: " + std::to_wstring(engine.GetAverageFPS());
	//uiManager.ReCreateUIData(uiComponent.UIName, averageFPS);

	// ADD TAGS TO XML FILES.
}

void UIRenderSystem::Render()
{
	static const ShaderManager& shaderManager = ShaderManager::GetInstanceRead();
	static const TextureManager& textureManager = TextureManager::GetInstanceRead();
	static UIManager& uiManager = UIManager::GetInstanceWrite();
	static const Engine& engine = Engine::GetInstanceRead();
	static Renderer& renderer = Renderer::GetInstanceWrite();

	for (const Entity entity : m_entities)
	{
		const TransformComponent& transformComponent = m_registry.GetComponentRead<TransformComponent>(entity);
		const UIComponent& uiComponent = m_registry.GetComponentRead<UIComponent>(entity);

		const std::wstring averageFPS = L"Average FPS: " + std::to_wstring(engine.GetAverageFPS());
		uiManager.ReCreateUIData(uiComponent.UIName, averageFPS);

		const ShaderData& shaderData = shaderManager.GetShaderDataRead(uiComponent.ShaderName);
		const TextureData& textureData = textureManager.GetTextureDataRead(uiComponent.TextureName);
		const UIData& uiData = uiManager.GetUIDataRead(uiComponent.UIName);
			
		renderer.UpdateUITextVertexBuffer(uiData);
		renderer.UpdatePerMeshConstantBuffer(transformComponent.Transform);
		renderer.EnableBlending();
		renderer.DrawUI(&uiData.Mesh, &shaderData, &textureData);
	}
}
