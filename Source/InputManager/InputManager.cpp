#include "PCH.h"
#include "InputManager.h"
#include "Core/Core.h"

void InputManager::Initialize()
{
	// Retrieve the main engine window handle
	const Window& window = Window::GetInstanceRead();
	HWND windowHandle = window.GetWindowHandle();

	// Construct the input devices and set the mouse device window.
	m_keyboard = std::make_unique<Keyboard>();
	m_mouse = std::make_unique<Mouse>();
	m_mouse->SetWindow(windowHandle);
}
