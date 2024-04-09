#include "PCH.h"
#include "Macros.h"

class Camera
{
	SINGLETON(Camera);

public:
	void Update(float deltaTime);
	XMMATRIX GetViewMatrix() const;

	float GetCameraSpeed() const { return m_cameraSpeed; }
	void SetCameraSpeed(float speed) { m_cameraSpeed = speed; }

private:
	void UpdateRotation(float deltaTime);
	void UpdatePosition(float deltaTime);

private:
	XMFLOAT3 m_position = { 0.0f, 0.0f, 0.0f };
	XMFLOAT3 m_rotation = { 0.0f, 0.0f, 0.0f };

	XMFLOAT3 m_right = { 1.0f, 0.0f, 0.0f };
	XMFLOAT3 m_up = { 0.0f, 1.0f, 0.0f };
	XMFLOAT3 m_forward = { 0.0f, 0.0f, 1.0f };

	float m_cameraSpeed = 5.0f;
};

