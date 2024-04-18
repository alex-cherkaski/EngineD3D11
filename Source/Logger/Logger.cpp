#include "PCH.h"
#include "Logger.h"

void Logger::Initialize()
{
	InitializeCriticalSection();
	InitializeLogFile();
}

void Logger::Shutdown()
{
	Log(LogType::Message, "Successfully shutdown logger.");

	// Release the resources for the critical section.
	DeleteCriticalSection(&m_criticalSection);
}

void Logger::InitializeCriticalSection()
{
	// Initialize the critical section. According to MS docs, this always succeeds.
	const BOOL initializeResult = InitializeCriticalSectionAndSpinCount(
		&m_criticalSection,		// Pointer to the critical section object to initialize.
		0x00000400				// The number of clock cycles to spin before going to sleep.
	);

	// Error check critical section initialization.
	ENGINE_ASSERT_W(initializeResult > 0, "Failed to initialize logger ciritical section.");
}

void Logger::InitializeLogFile()
{
	// Retrieve the current working directory.
	wchar_t buffer[MAX_PATH] = { TEXT('\0') };
	const DWORD charactersRead = GetCurrentDirectory(ARRAYSIZE(buffer), buffer);
	ENGINE_ASSERT_W(charactersRead > 0, "Failed to find current directory for log file.\n");
	m_logFilePath += buffer;
	m_logFilePath += TEXT("\\Log");

	// Create the directory for the log file, if it does not exist.
	CreateDirectory(m_logFilePath.c_str(), nullptr);
	m_logFilePath += TEXT("\\Log.txt");

	// Attempt to create the log file.
	const HANDLE logFile = CreateFile(
		m_logFilePath.c_str(),		// The name and path of the log file.
		GENERIC_WRITE,				// How the file should be created.
		0,							// No sharing between other processes.
		nullptr,					// No unique security attributes.
		CREATE_ALWAYS,				// How to treat the opening of this file.
		FILE_ATTRIBUTE_NORMAL,		// No unique file attributes.
		nullptr						// No unique templates for the file.
	);
	// Close the file handle.
	CloseHandle(logFile);

	// Attempt to make an entry in the log file.
	Log(LogType::Message, "Successfully initialized logger.");
}

void Logger::Log(Logger::LogType type, const char* format, ...)
{
	// Write the message into its buffer.
	char messageBuffer[1024] = { '\0' };
	va_list arglist;
	va_start(arglist, format);
#pragma warning(disable : 4996)
	vsprintf(messageBuffer, format, arglist);
#pragma warning(default : 4996)
	va_end(arglist);

	// Write the time into its buffer.
	SYSTEMTIME time = { 0 };
	GetSystemTime(&time);
	char timeBuffer[32] = { '\0' };
#pragma warning(disable : 4996)
	sprintf(timeBuffer, "[%s] [%02d:%02d:%05d] ", 
		type == LogType::Message ? "MESSAGE" : "ERROR", time.wHour, time.wSecond, time.wMilliseconds);
#pragma warning(default : 4996)

	// Concatenate the two strings together.
	std::string finalBuffer(timeBuffer);
	finalBuffer += messageBuffer;
	finalBuffer += '\n';

	// Lock the critical section.
	EnterCriticalSection(&m_criticalSection);

	// Attempt to open the log file.
	const HANDLE logFile = CreateFile(
		m_logFilePath.c_str(),		// The name and path of the log file.
		FILE_APPEND_DATA,			// How the file should be created.
		0,							// No sharing between other processes.
		nullptr,					// No unique security attributes.
		OPEN_EXISTING,				// How to treat the opening of this file.
		FILE_ATTRIBUTE_NORMAL,		// No unique file attributes.
		nullptr						// No unique templates for the file.
	);

	// Check for errors when creating the log file.
	ENGINE_ASSERT_W(logFile, "Failed to open engine log file.\n");

	// Attempt to write to the log file.
	DWORD bytesWritten = 0;
	const BOOL writeResult = WriteFile(
		logFile,						// The log file handle.
		finalBuffer.c_str(),			// The message to write.
		(DWORD)finalBuffer.length(),	// The number of bytes to write.
		&bytesWritten,					// The number of bytes written.
		nullptr							// Pointer to FILE_FLAG_OVERLAPPED struct if this flag was used to open the file.
	);

	// Error check log file write.
	ENGINE_ASSERT_W(bytesWritten == finalBuffer.length(), "Failed to write to engine log file.");

	// Close the file handle.
	CloseHandle(logFile);

	// Unlock the critical section.
	LeaveCriticalSection(&m_criticalSection);
}

