#pragma once
#include "ECS/System.h"

class GraphicsMeshRenderSystem final: public ISystem 
{
public:
	GraphicsMeshRenderSystem(Registry& registry);
	~GraphicsMeshRenderSystem() = default;

	// Inherited via ISystem
	void Initialize() override {}
	void Update(float deltaTime) override {}
	void Render() override;
};

