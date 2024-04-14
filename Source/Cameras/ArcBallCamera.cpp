#include "PCH.h"
#include "ArcBallCamera.h"
#include "InputManager/InputManager.h"
#include "Core/Core.h"

void ArcBallCamera::Update(float deltaTime)
{
	UpdatePosition(deltaTime);
	UpdateZoom(deltaTime);
}

XMMATRIX ArcBallCamera::GetViewMatrix() const
{
	// Retrieve the camera position and relevant basis vectors.
	const XMVECTOR position = { m_transform._41, m_transform._42, m_transform._43, m_transform._44 };
	const XMVECTOR forward = { m_transform._31, m_transform._32, m_transform._33, m_transform._34 };
	const XMVECTOR up = { m_transform._21, m_transform._22, m_transform._23, m_transform._24 };

	// Compute the look at direction of the camera.
	const XMVECTOR lookAtTarget = position + forward;

	// Create and return the final view matrix.
	return XMMatrixLookAtLH(position, lookAtTarget, up);
}

XMMATRIX ArcBallCamera::GetProjectionMatrix() const
{
	// Retrieve and computer the necessary window parameters.
	const Window& window = Window::GetInstanceRead();
	const float fovAngle = XMConvertToRadians(m_fovAngle);
	const float aspecRatio = (float)window.GetClientWidth() / window.GetClientHeight();
	constexpr float zNear = 0.01f;
	constexpr float zFar = 100.0f;

	// Construct the final projection matrix.
	return XMMatrixPerspectiveFovLH(fovAngle, aspecRatio, zNear, zFar);
}

void ArcBallCamera::UpdatePosition(float deltaTime)
{
	// Retrieve the input manager to query for the mouse state.
	const InputManager& inputManager = InputManager::GetInstanceRead();
	const Mouse::State mouseState = inputManager.GetMouseStateRead();

	// Check if there were any screen space changes with the mouse.
	const int deltaX = mouseState.x - m_lastMousePosition.x;
	const int deltaY = mouseState.y - m_lastMousePosition.y;

	// Yaw and pitch changes.
	float yaw = 0.0f;
	float pitch = 0.0f;

	// Check for changes in pitch.
	if (mouseState.leftButton && deltaY)
	{
		// Calculate if we are rotating clockwise or counter-clockwise.
		const int_fast8_t direction = mouseState.y < m_lastMousePosition.y ? -1 : 1;

		// Update the current x rotation angle.
		pitch = direction * deltaTime * m_angularSpeed;
	}

	// Check for changes in yaw.
	if (mouseState.leftButton && deltaX)
	{
		// Calculate if we are rotating clockwise or counter-clockwise.
		const int_fast8_t direction = mouseState.x < m_lastMousePosition.x ? -1 : 1;

		// Update the current y rotation angle.
		yaw = direction * deltaTime * m_angularSpeed;
	}

	// If there was any motion in the vertical or horizontal...
	if (mouseState.leftButton && (deltaX || deltaY))
	{
		// Matrix to move the camera to local space relative to the the target it is orbiting.
		const XMMATRIX localMatrix = XMMatrixTranslation(-m_targetPosition.x, -m_targetPosition.y, -m_targetPosition.z);

		// Matrix to rotate the camera about the target.
		const XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(
			XMConvertToRadians(pitch),
			XMConvertToRadians(yaw),
			0.0f
		);
		
		// Matrix to translate the camera back to its world space position.
		const XMMATRIX worldMatrix = XMMatrixTranslation(m_targetPosition.x, m_targetPosition.y, m_targetPosition.z);

		// Perform the matrix transformations from above.
		XMMATRIX transform = XMLoadFloat4x4(&m_transform);
		transform *= localMatrix * rotationMatrix * worldMatrix;
		XMStoreFloat4x4(&m_transform, transform);
	}

	// Cache the mouse position for the next frame.
	m_lastMousePosition = { mouseState.x, mouseState.y };
}

void ArcBallCamera::UpdateZoom(float deltaTime)
{
	// Get the input manager to retrieve the mouse state.
	InputManager& inputManager = InputManager::GetInstanceWrite();
	const Mouse::State mouseState = inputManager.GetMouseStateRead();

	// If the mouse wheel has been scrolled this frame...
	if (mouseState.scrollWheelValue)
	{
		// Determine if we scrolled up or down depending on the sign of the scroll wheel value.
		const int_fast8_t scrollDirection = mouseState.scrollWheelValue > 0 ? -1 : 1;

		// Adjust the camera FOV angle based on the scroll value.
		m_fovAngle += deltaTime * scrollDirection * m_zoomSpeed;
		m_fovAngle = ENGINE_CLAMP_F(m_fovAngle, 1.0f, 45.0f);
	}
}

