#include "PCH.h"
#include "Macros.h"
#include "Constants/Math.h"

class FirstPersonCamera
{
	SINGLETON(FirstPersonCamera);

public:
	void Update(float deltaTime);

	XMMATRIX GetViewMatrix() const;
	XMMATRIX GetProjectionMatrix() const;

	float GetLinearSpeed() const { return m_linearSpeed; }
	void SetLinearSpeed(float speed) { m_linearSpeed = speed; }

	float GetAngularSpeed() const { return m_angularSpeed; }
	void SetAngularSpeed(float speed) { m_angularSpeed = speed; }

	float GetMouseZoomSpeed() const { return m_mouseZoomSpeed; }
	float SetMouseZoomSpeed(float speed) { m_mouseZoomSpeed = speed; }

	float GetTriggerZoomSpeed() const { return m_triggerZoomSpeed; }
	float SetTriggerZoomSpeed(float speed) { m_triggerZoomSpeed = speed; }

private:
	void UpdateRotation(float deltaTime);
	void UpdateTranslation(float deltaTime);
	void UpdateZoom(float deltaTime);

private:
	XMFLOAT4X4 m_transform = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	XMINT2 m_lastMousePosition = { 0, 0 };

	float m_pitch = 0.0f;
	float m_yaw = 0.0f;

	float m_linearSpeed = 5.0f;
	float m_angularSpeed = 90.0f; // In degrees.
	float m_mouseZoomSpeed = 3000.0f;
	float m_triggerZoomSpeed = 30.0f;
	float m_fovAngle = 45.0f; // In degrees.
};

