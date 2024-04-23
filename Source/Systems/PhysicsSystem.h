#pragma once
#include "ECS/System.h"

class PhysicsSystem final : public ISystem
{
public:
	PhysicsSystem(Registry& registry);
	~PhysicsSystem() = default;

	// Inherited via ISystem.
	void Initialize() override {}
	void Update(float deltaTime) override;
	void Render() override {}
};

