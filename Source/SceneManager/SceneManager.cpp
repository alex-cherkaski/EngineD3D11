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
	//LoadScene(L"./Resources/Scenes/Light_Test.xml");
	//LoadScene(L"./Resources/Scenes/Texture_Test.xml");
	LoadScene(L"./Resources/Scenes/Texture_Mix_Test.xml");
}

void SceneManager::Update(float deltaTime)
{
	m_currentScene->Update(deltaTime);
}

void SceneManager::Render()
{
	m_currentScene->Render();
}

void SceneManager::LoadScene(const wchar_t* filePath)
{
	m_currentScene->Shutdown();
	m_currentScene = std::make_unique<Scene>(filePath);
}

