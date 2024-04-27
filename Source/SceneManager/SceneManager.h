#pragma once
#include "Macros.h"
#include "Scene.h"

struct TransformComponent;

class SceneManager final
{
	SINGLETON(SceneManager);

public:
	void Initialize();
	void Update(float deltaTime);
	void Render();

private:
	void LoadScene(const wchar_t* filePath);

private:
	std::unique_ptr<Scene> m_currentScene = nullptr;
};

