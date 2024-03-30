#pragma once
#include "PCH.h"
#include "Macros.h"

class Logger final
{
	SINGLETON(Logger);

public:
	enum LogType
	{
		Message,
		Error
	};

public:
	void Initialize();
	void Shutdown();

	void Log(Logger::LogType type, const char* format, ...);

private:
	void InitializeCriticalSection();
	void InitializeLogFile();

private:
	CRITICAL_SECTION m_criticalSection{};
	std::wstring m_logFilePath;
};
