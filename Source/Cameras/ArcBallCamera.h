#pragma once
#include "Macros.h"
#include "Constants/Math.h"

class ArcBallCamera
{
	SINGLETON(ArcBallCamera);

public:
	void Update(float deltaTime);

	XMMATRIX GetViewMatrix() const;
	XMMATRIX GetProjectionMatrix() const;

	void SetTargetPosition(const XMFLOAT3& targetPosition) { m_targetPosition = targetPosition; }
	const XMFLOAT3& GetTargetPosition() const { return m_targetPosition; }

private:
	void UpdatePosition(float deltaTime);
	void UpdateZoom(float deltaTime);

private:
	XMFLOAT4X4 m_transform = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	XMFLOAT3 m_rotation = { 0.0f, 0.0f, 0.0f };

	XMFLOAT3 m_targetPosition = { 0.0f, 0.0f, 0.0f };

	XMINT2 m_lastMousePosition = { 0, 0 };

	float m_fovAngle = 45.0f;

	float m_angularSpeed = 1000.0f;
	float m_zoomSpeed = 3000.0f;
};

