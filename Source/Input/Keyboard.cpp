#include "PCH.h"
#include "Keyboard.h"

bool IsKeyDown(VirtualKeyCode virtualKeyCode)
{
    const SHORT keyState = GetAsyncKeyState((int)virtualKeyCode);
    const bool isKeyDown = keyState & (1 << 15); // Most significant bit is 1 if key is up.
    return isKeyDown;
}

bool IsKeyUp(VirtualKeyCode virtualKeyCode)
{
	const SHORT keyState = GetAsyncKeyState((int)virtualKeyCode);
	const bool isKeyDown = keyState & 1; // Least significant bit is 1 if key is down.
	return isKeyDown;
}

