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
	const XMVECTOR position = { m_transform._41, m_transform._42, m_transform._43, 0.0f };
	const XMVECTOR forward = { m_transform._31, m_transform._32, m_transform._33, 0.0f };
	const XMVECTOR up = { m_transform._21, m_transform._22, m_transform._23, 0.0f };

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
	const GamePad::State gamePadState = inputManager.GetGamePadState(0);

	// Check for mouse state changes.
	const int deltaX = mouseState.x - m_lastMousePosition.x;
	const int deltaY = mouseState.y - m_lastMousePosition.y;

	// Check for right thumb stick state changes.
	const bool xRightStickPositive = gamePadState.thumbSticks.rightX > 0;
	const bool xRightStickNegative = gamePadState.thumbSticks.rightX < 0;
	const bool yRightStickPositive = gamePadState.thumbSticks.rightY > 0;
	const bool yRightStickNegative = gamePadState.thumbSticks.rightY < 0;

	// Flags to cache whether there were any mouse or thumb stick changes on this frame.
	const bool mouseButtonPressed = mouseState.leftButton && (deltaX || deltaY);
	const bool rightStickMoved = gamePadState.IsConnected() && (xRightStickPositive || xRightStickNegative || yRightStickPositive || yRightStickNegative);

	// If there was a change in camera pitch or yaw update camera basis vectors.
	if (mouseButtonPressed || rightStickMoved)
	{
		// Update the cached camera rotation based on the new mouse or game pad input.
		if (mouseState.leftButton && deltaY > 0 || yRightStickPositive)
			m_pitch -= deltaTime * m_angularSpeed;
		if (mouseState.leftButton && deltaY < 0 || yRightStickNegative)
			m_pitch += deltaTime * m_angularSpeed;
		if (mouseState.leftButton && deltaX > 0 || xRightStickPositive)
			m_yaw += deltaTime * m_angularSpeed;
		if (mouseState.leftButton && deltaX < 0 || xRightStickNegative)
			m_yaw -= deltaTime * m_angularSpeed;

		// Construct the rotation and world matrices to transform the camera.
		const XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_pitch), XMConvertToRadians(m_yaw), 0.0f);
		const XMMATRIX worldMatrix = XMMatrixTranslation(m_transform._41, m_transform._42, m_transform._43);

		// Update the camera transform based on the camera position, pitch, and yaw.
		XMMATRIX transform = XMMatrixIdentity();
		transform *= rotationMatrix;
		transform *= worldMatrix;
		XMStoreFloat4x4(&m_transform, transform);

		// Update the last mouse position for this frame.
		m_lastMousePosition = { mouseState.x, mouseState.y };
	}
}

void FirstPersonCamera::UpdateTranslation(float deltaTime)
{
	// Get the keyboard and game pad states to query for input changes.
	const InputManager& inputManager = InputManager::GetInstanceRead();
	const Keyboard::State keyboardState = inputManager.GetKeyboardStateRead();
	const GamePad::State gamePadState = inputManager.GetGamePadState(0);

	// Get the left thumb stick states.
	const bool xLeftStickPositive = gamePadState.thumbSticks.leftX > 0;
	const bool xLeftStickNegative = gamePadState.thumbSticks.leftX < 0;
	const bool yLeftStickPositive = gamePadState.thumbSticks.leftY > 0;
	const bool yLeftStickNegative = gamePadState.thumbSticks.leftY < 0;

	// Record if a game pad stick was moved or a keyboard button was pressed.
	const bool movementKeyPressed = keyboardState.W || keyboardState.S || keyboardState.D || keyboardState.A;
	const bool leftStickMoved = gamePadState.IsConnected() && (xLeftStickPositive || xLeftStickNegative || yLeftStickPositive || yLeftStickNegative);

	// If there was any movement related input registered for this frame...
	if (movementKeyPressed || leftStickMoved)
	{
		// Construct a SIMD camera transform matrix to update.
		XMMATRIX transform = XMLoadFloat4x4(&m_transform);

		// Update the camera position if the relevant key was pressed.
		if (keyboardState.W || yLeftStickPositive)
			transform.r[3] += deltaTime * m_linearSpeed * transform.r[2];
		if (keyboardState.S || yLeftStickNegative)
			transform.r[3] -= deltaTime * m_linearSpeed * transform.r[2];
		if (keyboardState.D || xLeftStickPositive)
			transform.r[3] += deltaTime * m_linearSpeed * transform.r[0];
		if (keyboardState.A || xLeftStickNegative)
			transform.r[3] -= deltaTime * m_linearSpeed * transform.r[0];

		// Write back the updated camera transform matrix.
		XMStoreFloat4x4(&m_transform, transform);
	}
}

void FirstPersonCamera::UpdateZoom(float deltaTime)
{
	// Get the input manager to retrieve the mouse state.
	InputManager& inputManager = InputManager::GetInstanceWrite();
	const Mouse::State mouseState = inputManager.GetMouseStateRead();
	const GamePad::State gamePadState = inputManager.GetGamePadState(0);

	// Check for any mouse scroll wheel or game pad trigger changes.
	const bool scrollWheelUp = mouseState.scrollWheelValue > 0;
	const bool scrollWheelDown = mouseState.scrollWheelValue < 0;
	const bool rightTriggerPressed = gamePadState.triggers.right > 0.0f;
	const bool leftTriggerPressed = gamePadState.triggers.left > 0.0f;

	// Cached flags for mouse wheel or game pad trigger changes.
	const bool mouseWheelScrolled = scrollWheelUp || scrollWheelDown;
	const bool gamePadTriggerPressed = rightTriggerPressed || leftTriggerPressed;

	// If the mouse wheel was scrolled or a game pad trigger is pressed...
	if (mouseWheelScrolled || gamePadTriggerPressed)
	{
		// Adjust the FOV angle based on any mouse wheel or game pad trigger changes.
		if (scrollWheelUp)
			m_fovAngle -= deltaTime * m_mouseZoomSpeed;
		if (scrollWheelDown)
			m_fovAngle += deltaTime * m_mouseZoomSpeed;
		if (rightTriggerPressed)
			m_fovAngle -= deltaTime * m_triggerZoomSpeed;
		if (leftTriggerPressed)
			m_fovAngle += deltaTime * m_triggerZoomSpeed;

		// Clamp the FOV angle to the range of [1, 45] degrees.
		m_fovAngle = ENGINE_CLAMP_F(m_fovAngle, 1.0f, 45.0f);
	}
}
