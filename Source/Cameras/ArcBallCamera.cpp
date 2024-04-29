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
	// Get the camera position and basis vectors to compute the look at matrix.
	const XMVECTOR position = { m_transform._41, m_transform._42, m_transform._43 };
	const XMVECTOR forward = { m_transform._31, m_transform._32, m_transform._33 };
	const XMVECTOR up = { m_transform._21, m_transform._22, m_transform._23 };

	// Compute the look at direction of the camera.
	const XMVECTOR lookAtTarget = position + forward;

	// Create and return the final view matrix.
	return XMMatrixLookAtLH(position, lookAtTarget, up);
}

XMMATRIX ArcBallCamera::GetPerspectiveMatrix() const
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

XMMATRIX ArcBallCamera::GetOrthographicMatrix() const
{
	// Retrieve the window frame buffer dimensions to create the orthographic matrix.
	const Window& window = Window::GetInstanceRead();
	const float viewWidth = (float)window.GetClientWidth();
	const float viewHeight = (float)window.GetClientHeight();
	const float nearPlane = 0.01f;
	const float farPlane = 1.0f;

	// Create the orthographic matrix.
	return XMMatrixOrthographicLH(viewWidth, viewHeight, nearPlane, farPlane);
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
		// The pitch and yaw for this 
		float deltaPitch = 0.0f;
		float deltaYaw = 0.0f;

		// Check for changes in input to adjust the camera pitch or yaw.
		{
			// Pitch Up.
			if (mouseState.leftButton && deltaY > 0)
				deltaPitch -= deltaTime * m_mouseAngularSpeed;
			if (yLeftStickPositive)
				deltaPitch -= deltaTime * m_gamePadAngularSpeed;

			// Pitch Down.
			if (mouseState.leftButton && deltaY < 0)
				deltaPitch += deltaTime * m_mouseAngularSpeed;
			if (yLeftStickNegative)
				deltaPitch += deltaTime * m_gamePadAngularSpeed;

			// Yaw Right.
			if (mouseState.leftButton && deltaX > 0)
				deltaYaw -= deltaTime * m_mouseAngularSpeed;
			if (xLeftStickPositive)
				deltaYaw -= deltaTime * m_gamePadAngularSpeed;

			// Yaw Left.
			if (mouseState.leftButton && deltaX < 0)
				deltaYaw += deltaTime * m_mouseAngularSpeed;
			if (xLeftStickNegative)
				deltaYaw += deltaTime * m_gamePadAngularSpeed;
		}


		// Construct the transformation matrices to go to local space, rotate, and come back to world space.
		const XMMATRIX localSpaceMatrix = XMMatrixTranslation(-m_target.x, -m_target.y, -m_target.z);
		const XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(XMConvertToRadians(deltaPitch), XMConvertToRadians(deltaYaw), 0.0f);
		const XMMATRIX worldSpaceMatrix = XMMatrixTranslation(m_target.x, m_target.y, m_target.z);

		// Apply the model, rotation, world transformation to the camera transform.
		XMMATRIX transform = XMLoadFloat4x4(&m_transform);
		transform *= localSpaceMatrix * rotationMatrix * worldSpaceMatrix;
		XMStoreFloat4x4(&m_transform, transform);

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

