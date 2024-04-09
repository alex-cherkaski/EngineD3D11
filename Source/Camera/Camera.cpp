#include "PCH.h"
#include "Camera.h"
#include "Input/Keyboard.h"

void Camera::Update(float deltaTime)
{
	UpdateRotation(deltaTime);
	UpdatePosition(deltaTime);
}

XMMATRIX Camera::GetViewMatrix() const
{
	const XMVECTOR position = XMLoadFloat3(&m_position);
	const XMVECTOR forward = XMLoadFloat3(&m_forward);
	const XMVECTOR lookAtTarget = position + forward;
	const XMVECTOR up = XMLoadFloat3(&m_up);

	return XMMatrixLookAtLH(position, lookAtTarget, up);
}

void Camera::UpdateRotation(float deltaTime)
{

}

void Camera::UpdatePosition(float deltaTime)
{
	// Retrieve the relevant camera components.
	XMVECTOR position = XMLoadFloat3(&m_position);
	const XMVECTOR forward = XMLoadFloat3(&m_forward);
	const XMVECTOR right = XMLoadFloat3(&m_right);

	// Update the camera position if the relevant key was pressed.
	if (IsKeyDown(VirtualKeyCode::W))
		position += deltaTime * m_cameraSpeed * forward;
	if (IsKeyDown(VirtualKeyCode::S))
		position += deltaTime * (-1) * m_cameraSpeed * forward;
	if (IsKeyDown(VirtualKeyCode::D))
		position += deltaTime * m_cameraSpeed * right;
	if (IsKeyDown(VirtualKeyCode::A))
		position += deltaTime * (-1) * m_cameraSpeed * right;

	// Write back the updated camera position.
	XMStoreFloat3(&m_position, position);
}
