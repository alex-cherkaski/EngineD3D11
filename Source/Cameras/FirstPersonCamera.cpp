#include "PCH.h"
#include "Core/Core.h"
#include "FirstPersonCamera.h"
#include "InputManager/InputManager.h"

void FirstPersonCamera::Update(float deltaTime)
{
	UpdateTranslation(deltaTime);
	UpdateRotation(deltaTime);
	UpdateZoom(deltaTime);
}

XMMATRIX FirstPersonCamera::GetViewMatrix() const
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

	// Variables to record changes to the pitch and yaw of the camera for this frame.
	float pitch = 0.0f;
	float yaw = 0.0f;

	// Check for changes in camera pitch.
	if (mouseState.leftButton && deltaY)
	{
		// Determine if we are to rotate clockwise or counter-clockwise.
		const int_fast8_t pitchDirection = mouseState.y < m_lastMousePosition.y ? 1 : -1;

		// Update the amount of pitch the camera should have.
		pitch += deltaTime * pitchDirection * m_angularSpeed;
	}

	// Check for changes in camera yaw.
	if (mouseState.leftButton && deltaX)
	{
		// Determine if we are to rotate clockwise or counter-clockwise.
		const int_fast8_t yawDirection = mouseState.x < m_lastMousePosition.x ? -1 : 1;

		// Update the amount of yaw the camera should have.
		yaw += deltaTime * yawDirection * m_angularSpeed;
	}

	// If there was a change in camera pitch or yaw update camera basis vectors.
	if (mouseState.leftButton && (deltaY || deltaX))
	{
		// Translation matrix that will move the camera back to local space.
		const XMMATRIX localMatrix = XMMatrixTranslation(-m_transform._41, -m_transform._42, -m_transform._43);

		// Rotation matrix that will update the camera rotation.
		const XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(
			XMConvertToRadians(pitch),
			XMConvertToRadians(yaw),
			0.0f
		);

		// Translation matrix that will move the camera back to world space.
		const XMMATRIX worldMatrix = XMMatrixTranslation(m_transform._41, m_transform._42, m_transform._43);

		// Apply the above transformations in order to rotate the camera about its own origin.
		XMMATRIX transform = XMLoadFloat4x4(&m_transform);
		transform *= localMatrix * rotationMatrix * worldMatrix;
		XMStoreFloat4x4(&m_transform, transform);
	}

	// Update the last mouse position for this frame.
	m_lastMousePosition = { mouseState.x, mouseState.y };
}

void FirstPersonCamera::UpdateTranslation(float deltaTime)
{
	// Get the keyboard state from the input manager.
	const InputManager& inputManager = InputManager::GetInstanceRead();
	const Keyboard::State keyboardState = inputManager.GetKeyboardStateRead();

	// Construct the SIMD the camera transform and translation matrices to do the updating.
	XMMATRIX transform = XMLoadFloat4x4(&m_transform);
	XMMATRIX translation = XMMatrixIdentity();

	// Update the camera position if the relevant key was pressed.
	if (keyboardState.W)
		translation.r[3] += deltaTime * m_linearSpeed * transform.r[2];
	if (keyboardState.S)
		translation.r[3] += (-1) * deltaTime * m_linearSpeed * transform.r[2];
	if (keyboardState.D)
		translation.r[3] += deltaTime * m_linearSpeed * transform.r[0];
	if (keyboardState.A)
		translation.r[3] += (-1) * deltaTime * m_linearSpeed * transform.r[0];

	// If there was any movement related input registered for this frame...
	if (keyboardState.W || keyboardState.S || keyboardState.D || keyboardState.A)
	{
		// Apply the translation matrix to the camera world transform.
		transform *= translation;
		XMStoreFloat4x4(&m_transform, transform);
	}
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
	}
}
