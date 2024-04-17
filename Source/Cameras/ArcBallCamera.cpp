#include "PCH.h"
#include "ArcBallCamera.h"
#include "Core/Core.h"
#include "InputManager/InputManager.h"

void ArcBallCamera::Update(float deltaTime)
{
	UpdatePosition(deltaTime);
	UpdateZoom(deltaTime);
}

XMMATRIX ArcBallCamera::GetViewMatrix() const
{
	// Compute the rotation matrix to rotate the camera position about the target position.
	const XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_rotation.x), XMConvertToRadians(m_rotation.y), 0.0f);

	// Retrieve the camera and target world positions to offset the camera position before performing the rotation.
	XMVECTOR position = XMLoadFloat3(&m_position);
	XMVECTOR target = XMLoadFloat3(&m_target);

	// Translate the camera to target space, perform the rotation, and translate the camera back to world space.
	position -= target;
	position = XMVector3Transform(position, rotationMatrix);
	position += target;

	// Retrieve the camera position and relevant basis vectors.
	const XMVECTOR forward = ComputeForward(position);
	const XMVECTOR up = ComputeUp();

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
	// Get the mouse state from the input manager.
	const InputManager& inputManager = InputManager::GetInstanceRead();
	const Mouse::State mouseState = inputManager.GetMouseStateRead();
	const GamePad::State gamePadState = inputManager.GetGamePadState(0);

	// Check for mouse state changes.
	const int deltaX = mouseState.x - m_lastMousePosition.x;
	const int deltaY = mouseState.y - m_lastMousePosition.y;

	// Check for right thumb stick state changes.
	const bool xLeftStickPositive = gamePadState.thumbSticks.rightX > 0;
	const bool xLeftStickNegative = gamePadState.thumbSticks.rightX < 0;
	const bool yLeftStickPositive = gamePadState.thumbSticks.rightY > 0;
	const bool yLeftStickNegative = gamePadState.thumbSticks.rightY < 0;

	// Flags to cache whether there were any mouse or thumb stick changes on this frame.
	const bool mouseButtonPressed = mouseState.leftButton && (deltaX || deltaY);
	const bool rightStickMoved = gamePadState.IsConnected() && (xLeftStickPositive || xLeftStickNegative || yLeftStickPositive || yLeftStickNegative);

	// If there was a change in camera pitch or yaw update camera basis vectors.
	if (mouseButtonPressed || rightStickMoved)
	{
		// Update the cached camera rotation based on the new mouse or game pad input.
		if (mouseState.leftButton && deltaY > 0 || yLeftStickPositive)
			m_rotation.x -= deltaTime * m_angularSpeed;
		if (mouseState.leftButton && deltaY < 0 || yLeftStickNegative)
			m_rotation.x += deltaTime * m_angularSpeed;
		if (mouseState.leftButton && deltaX > 0 || xLeftStickPositive)
			m_rotation.y += deltaTime * m_angularSpeed;
		if (mouseState.leftButton && deltaX < 0 || xLeftStickNegative)
			m_rotation.y -= deltaTime * m_angularSpeed;

		m_rotation.x = ENGINE_CLAMP_F(m_rotation.x, -89.0f, 89.0f);

		// Update the last mouse position for this frame.
		m_lastMousePosition = { mouseState.x, mouseState.y };
	}
}

void ArcBallCamera::UpdateZoom(float deltaTime)
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

XMVECTOR ArcBallCamera::ComputeForward(const XMVECTOR position) const
{
	// Compute the forward vector of the camera.
	const XMVECTOR target = XMLoadFloat3(&m_target);
	const XMVECTOR forward = target - position;
	return forward;
}

XMVECTOR ArcBallCamera::ComputeRight() const
{
	// Compute the right vector of the camera.
	const XMVECTOR forward = XMLoadFloat3(&m_forward);
	const XMVECTOR right = XMVector3Cross(UP_VECTOR3_V, forward);
	return right;
}

XMVECTOR ArcBallCamera::ComputeUp() const
{
	// Compute the up vector of the camera.
	const XMVECTOR forward = XMLoadFloat3(&m_forward);
	const XMVECTOR right = XMLoadFloat3(&m_right);
	const XMVECTOR up = XMVector3Cross(forward, right);
	return up;
}

