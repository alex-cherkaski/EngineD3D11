#pragma once
#include "ECS/System.h"

class UIRenderSystem final : public ISystem
{
public:
	UIRenderSystem(Registry& registry);
	~UIRenderSystem();

	// Inherited via ISystem.
	void Initialize() override {}
	void Update(float deltaTime) override;
	void Render() override;
};

