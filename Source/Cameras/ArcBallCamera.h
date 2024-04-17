#include "PCH.h"
#include "Macros.h"
#include "Constants/Math.h"

class ArcBallCamera
{
	SINGLETON(ArcBallCamera);

public:
	void Update(float deltaTime);

	XMMATRIX GetViewMatrix() const;
	XMMATRIX GetProjectionMatrix() const;

	const XMFLOAT3& GetCameraPosition() const { return m_position; }
	void SetCameraPosition(const XMFLOAT3& cameraPosition) { m_position = cameraPosition; }

	const XMFLOAT3& GetTargetPosition() const { return m_target; }
	void SetTargetPosition(const XMFLOAT3& targetPosition) { m_target = targetPosition; }

	float GetLinearSpeed() const { return m_linearSpeed; }
	void SetLinearSpeed(float speed) { m_linearSpeed = speed; }

	float GetAngularSpeed() const { return m_angularSpeed; }
	void SetAngularSpeed(float speed) { m_angularSpeed = speed; }

	float GetMouseZoomSpeed() const { return m_mouseZoomSpeed; }
	float SetMouseZoomSpeed(float speed) { m_mouseZoomSpeed = speed; }

	float GetTriggerZoomSpeed() const { return m_triggerZoomSpeed; }
	float SetTriggerZoomSpeed(float speed) { m_triggerZoomSpeed = speed; }

private:
	void UpdatePosition(float deltaTime);
	void UpdateZoom(float deltaTime);

	XMVECTOR ComputeForward(const XMVECTOR position) const;
	XMVECTOR ComputeRight() const;
	XMVECTOR ComputeUp() const;

private:
	XMFLOAT3 m_position = { 0.0f, 0.0f, 0.0f };
	XMFLOAT3 m_target = { 0.0f, 0.0f, 0.0f };

	XMFLOAT3 m_forward = FORWARD_VECTOR3_F;
	XMFLOAT3 m_right = RIGHT_VECTOR3_F;
	XMFLOAT3 m_up = UP_VECTOR3_F;

	XMINT2 m_lastMousePosition = { 0, 0 };

	XMFLOAT2 m_rotation = { 0.0f, 0.0f }; // In degrees.

	float m_linearSpeed = 5.0f;
	float m_angularSpeed = 90.0f; // In degrees.
	float m_mouseZoomSpeed = 3000.0f;
	float m_triggerZoomSpeed = 30.0f;
	float m_fovAngle = 45.0f; // In degrees.
};

