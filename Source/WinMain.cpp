#include "PCH.h"
#include "Core/Core.h"

int WINAPI wWinMain(
	_In_ HINSTANCE hInstance,			// Handle to the application instance.
	_In_opt_ HINSTANCE hPrevInstance,	// Handle to the previous application instance.
	_In_ LPWSTR lpCmdLine,				// Pointer to command line arguments.
	_In_ int nShowCmd					// ID specifying how the window should be shown.
)
{
	UNREFERENCED_PARAMETER(hPrevInstance);

	Engine& engine = Engine::GetInstanceWrite();
	engine.Initialize(hInstance, lpCmdLine, nShowCmd);
	engine.Run();
	engine.Shutdown();

	return EXIT_SUCCESS;
}