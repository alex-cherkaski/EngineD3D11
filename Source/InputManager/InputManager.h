#pragma once
#include "PCH.h"
#include "Macros.h"

class InputManager final
{
	SINGLETON(InputManager);

public:
	void Initialize();

	// Keyboard
	Keyboard::State GetKeyboardStateRead() const { return m_keyboard->GetState(); }
	bool IsKeyboardConnected() const { return m_keyboard->IsConnected(); }
	void ResetKeyboardState() { m_keyboard->Reset(); }

	// Mouse
	Mouse::State GetMouseStateRead() const { return m_mouse->GetState(); }
	bool IsMouseConnected() const { return m_mouse->IsConnected(); }
	bool IsMouseVisible() const { return m_mouse->IsVisible(); }
	void SetMouseVisible(bool isVisible) { m_mouse->SetVisible(isVisible); }
	void ResetMouseScrollWheelValue() { m_mouse->ResetScrollWheelValue(); }
	void SignalMouseEndOfInputFrame() { m_mouse->EndOfInputFrame(); }

private:
	std::unique_ptr<Keyboard> m_keyboard = nullptr;
	std::unique_ptr<Mouse> m_mouse = nullptr;
};

