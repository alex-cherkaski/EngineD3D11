#include "PCH.h"
#include "Components/Components.h"
#include "ECS/Registry.h"
#include "PhysicsSystem.h"

PhysicsSystem::PhysicsSystem(Registry& registry)
	: ISystem::ISystem(registry)
{
	RequireComponent<TransformComponent>();
	RequireComponent<PhysicsComponent>();
}

void PhysicsSystem::Update(float deltaTime)
{
	for (const Entity entity : m_entities)
	{
		// Retrieve the relevant components for physics update.
		TransformComponent& transformComponent = m_registry.GetComponentWrite<TransformComponent>(entity);
		const PhysicsComponent& physicsComponent = m_registry.GetComponentRead<PhysicsComponent>(entity);

		// Rotation Update.
		{
			// Calculate the changes in roll, pitch, and yaw for this frame.
			const float pitch = XMConvertToRadians(physicsComponent.AngularVelocity.x * deltaTime);
			const float yaw = XMConvertToRadians(physicsComponent.AngularVelocity.y * deltaTime);
			const float roll = XMConvertToRadians(physicsComponent.AngularVelocity.z * deltaTime);

			const float xPosition = transformComponent.Transform._41;
			const float yPosition = transformComponent.Transform._42;
			const float zPosition = transformComponent.Transform._43;

			const XMMATRIX localSpace = XMMatrixTranslation(-xPosition, -yPosition, -zPosition);
			const XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
			const XMMATRIX worldSpace = XMMatrixTranslation(xPosition, yPosition, zPosition);

			// Update the transform component's rotation.
			XMMATRIX transform = XMLoadFloat4x4(&transformComponent.Transform);
			transform *= localSpace * rotationMatrix * worldSpace;
			XMStoreFloat4x4(&transformComponent.Transform, transform);
		}

		// Translation Update.
		{
			// Calculate the translation in the x, y, and z dimensions.
			const float xTranslation = physicsComponent.LinearVelocity.x * deltaTime;
			const float yTranslation = physicsComponent.LinearVelocity.y * deltaTime;
			const float zTranslation = physicsComponent.LinearVelocity.z * deltaTime;

			// Construct the translation matrix.
			const XMMATRIX translationMatrix = XMMatrixTranslation(xTranslation, yTranslation, zTranslation);

			// Update the transform component's translation.
			XMMATRIX transform = XMLoadFloat4x4(&transformComponent.Transform);
			transform *= translationMatrix;
			XMStoreFloat4x4(&transformComponent.Transform, transform);
		}
	}
}
