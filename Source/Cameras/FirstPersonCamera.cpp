#include "PCH.h"
#include "Core/Core.h"
#include "FirstPersonCamera.h"
#include "InputManager/InputManager.h"

void FirstPersonCamera::Update(float deltaTime)
{
	UpdateRotation(deltaTime);
	UpdatePosition(deltaTime);
	UpdateZoom(deltaTime);
}

XMMATRIX FirstPersonCamera::GetViewMatrix() const
{
	// Retrieve the camera position and relevant basis vectors.
	const XMVECTOR position = XMLoadFloat3(&m_position);
	const XMVECTOR forward = XMLoadFloat3(&m_forward);
	const XMVECTOR up = XMLoadFloat3(&m_up);

	// Compute the look at direction of the camera.
	const XMVECTOR lookAtTarget = position + forward;

	// Create and return the final view matrix.
	return XMMatrixLookAtLH(position, lookAtTarget, up);
}

XMMATRIX FirstPersonCamera::GetProjectionMatrix() const
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

void FirstPersonCamera::UpdateRotation(float deltaTime)
{
	// Get the mouse state from the input manager.
	const InputManager& inputManager = InputManager::GetInstanceRead();
	const Mouse::State mouseState = inputManager.GetMouseStateRead();

	// Compute changes in x and y mouse positions.
	const int deltaX = mouseState.x - m_lastMousePosition.x;
	const int deltaY = mouseState.y - m_lastMousePosition.y;

	// Check for changes in camera pitch.
	if (mouseState.leftButton && deltaY)
	{
		// Determine if we are to rotate clockwise or counter-clockwise.
		const int_fast8_t pitchDirection = mouseState.y < m_lastMousePosition.y ? 1 : -1;

		// Update the amount of pitch the camera should have.
		m_rotation.x += deltaTime * pitchDirection * m_angularSpeed;
	}

	// Check for changes in camera yaw.
	if (mouseState.leftButton && deltaX)
	{
		// Determine if we are to rotate clockwise or counter-clockwise.
		const int_fast8_t yawDirection = mouseState.x < m_lastMousePosition.x ? -1 : 1;

		// Update the amount of yaw the camera should have.
		m_rotation.y += deltaTime * yawDirection * m_angularSpeed;
	}

	// If there was a change in camera pitch or yaw update camera basis vectors.
	if (mouseState.leftButton && (deltaY || deltaX))
	{
		// Create the rotation matrix about the x-axis.
		const XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(
			XMConvertToRadians(m_rotation.x),
			XMConvertToRadians(m_rotation.y),
			XMConvertToRadians(m_rotation.z));

		// Construct a SIMD version of the vector, rotate it, and write back the rotated right vector.
		XMVECTOR right = XMLoadFloat3(&RIGHT_VECTOR);
		right = XMVector3Transform(right, rotationMatrix);
		XMStoreFloat3(&m_right, right);

		// Construct a SIMD version of the vector, rotate it, and write back the rotated right vector.
		XMVECTOR up = XMLoadFloat3(&UP_VECTOR);
		up = XMVector3Transform(up, rotationMatrix);
		XMStoreFloat3(&m_up, up);

		// Construct a SIMD version of the vector, rotate it, and write back the rotated forward vector.
		XMVECTOR forward = XMLoadFloat3(&FORWARD_VECTOR);
		forward = XMVector3Transform(forward, rotationMatrix);
		XMStoreFloat3(&m_forward, forward);
	}

	// Update the last mouse position for this frame.
	m_lastMousePosition = { mouseState.x, mouseState.y };
}

void FirstPersonCamera::UpdatePosition(float deltaTime)
{
	// Get the keyboard state from the input manager.
	const InputManager& inputManager = InputManager::GetInstanceRead();
	const Keyboard::State keyboardState = inputManager.GetKeyboardStateRead();

	// Retrieve the relevant camera components.
	XMVECTOR position = XMLoadFloat3(&m_position);
	const XMVECTOR forward = XMLoadFloat3(&m_forward);
	const XMVECTOR right = XMLoadFloat3(&m_right);

	// Update the camera position if the relevant key was pressed.
	if (keyboardState.W)
		position += deltaTime * m_linearSpeed * forward;
	if (keyboardState.S)
		position += deltaTime * (-1) * m_linearSpeed * forward;
	if (keyboardState.D)
		position += deltaTime * m_linearSpeed * right;
	if (keyboardState.A)
		position += deltaTime * (-1) * m_linearSpeed * right;

	// Write back the updated camera position.
	XMStoreFloat3(&m_position, position);
}

void FirstPersonCamera::UpdateZoom(float deltaTime)
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

		// Reset the state of the scroll wheel to check for up or down scrolls on the next frame.
		inputManager.ResetMouseScrollWheelValue();
	}
}
