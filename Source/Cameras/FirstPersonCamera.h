#include "PCH.h"
#include "Macros.h"
#include "Constants/Math.h"

class FirstPersonCamera
{
	SINGLETON(FirstPersonCamera);

public:
	void Update(float deltaTime);
	XMMATRIX GetViewMatrix() const;

	float GetLinearSpeed() const { return m_linearSpeed; }
	void SetLinearSpeed(float speed) { m_linearSpeed = speed; }

	float GetAngularSpeed() const { return m_angularSpeed; }
	void SetAngularSpeed(float speed) { m_angularSpeed = speed; }

private:
	void UpdateRotation(float deltaTime);
	void UpdatePosition(float deltaTime);

private:
	XMFLOAT3 m_position = { 0.0f, 0.0f, 0.0f };
	XMFLOAT3 m_rotation = { 0.0f, 0.0f, 0.0f };

	XMFLOAT3 m_right = RIGHT_VECTOR;
	XMFLOAT3 m_up = UP_VECTOR;
	XMFLOAT3 m_forward = FORWARD_VECTOR;

	XMINT2 m_lastMousePosition = { 0, 0 };

	float m_linearSpeed = 5.0f;
	float m_angularSpeed = 90.0f; // In degrees.
};

