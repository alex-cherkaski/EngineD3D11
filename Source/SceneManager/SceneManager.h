#pragma once
#include "Macros.h"

class SceneManager final
{
	SINGLETON(SceneManager);

public:
	void Initialize();
	void Update(float deltaTime);
	void Render();

private:
	void CreateSceneData();
	void CreateEntities();
	void CreateSystems();
};

