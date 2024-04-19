#include "PCH.h"
#include "Constants/Math.h"
#include "Macros.h"

class ArcBallCamera
{
	SINGLETON(ArcBallCamera);

public:
	void Update(float deltaTime);

	XMMATRIX GetViewMatrix() const;
	XMMATRIX GetProjectionMatrix() const;

	const XMFLOAT3 GetCameraPosition() const { return { m_transform._41, m_transform._42, m_transform._43, }; }
	void SetCameraPosition(const XMFLOAT3& cameraPosition) { m_transform._41 = cameraPosition.x; m_transform._42 = cameraPosition.y; m_transform._43 = cameraPosition.z; }

	const XMFLOAT3& GetTargetPosition() const { return m_target; }
	void SetTargetPosition(const XMFLOAT3& targetPosition) { m_target = targetPosition; }

	float GetLinearSpeed() const { return m_linearSpeed; }
	void SetLinearSpeed(float speed) { m_linearSpeed = speed; }

	float GetGamePadAngularSpeed() const { return m_gamePadAngularSpeed; }
	void SetGamePadAngularSpeed(float speed) { m_gamePadAngularSpeed = speed; }

	float GetMouseAngularSpeed() const { return m_mouseAngularSpeed; }
	void SetMouseAngularSpeed(float speed) { m_mouseAngularSpeed = speed; }

	float GetMouseZoomSpeed() const { return m_mouseZoomSpeed; }
	float SetMouseZoomSpeed(float speed) { m_mouseZoomSpeed = speed; }

	float GetTriggerZoomSpeed() const { return m_triggerZoomSpeed; }
	float SetTriggerZoomSpeed(float speed) { m_triggerZoomSpeed = speed; }

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

	XMFLOAT3 m_target = { 0.0f, 0.0f, 0.0f };

	XMINT2 m_lastMousePosition = { 0, 0 };

	float m_linearSpeed = 5.0f;				// In units/second.
	float m_gamePadAngularSpeed = 90.0f;	// In degrees/second.
	float m_mouseAngularSpeed = 360.0f;		// In degrees/second.
	float m_mouseZoomSpeed = 3000.0f;		// In degrees/second
	float m_triggerZoomSpeed = 30.0f;		// In degrees/second.
	float m_fovAngle = 45.0f;				// In degrees.
};

