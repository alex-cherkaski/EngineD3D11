#pragma once
#include "PCH.h"

#define NO_COPY(CLASS) \
	CLASS(const CLASS&) = delete; \
	CLASS& operator=(const CLASS&) = delete

#define NO_MOVE(CLASS) \
	CLASS(CLASS&&) = delete; \
	CLASS& operator=(CLASS&&) = delete; \

#define DEFAULT_COPY(CLASS) \
	CLASS(const CLASS&) = default; \
	CLASS& operator=(const CLASS&) = default

#define DEFAULT_MOVE(CLASS) \
	CLASS(CLASS&& other) noexcept = default; \
	CLASS& operator=(CLASS&& other) noexcept = default

#define SINGLETON(CLASS) \
	NO_COPY(CLASS); \
	NO_MOVE(CLASS); \
	private: \
		CLASS() = default; \
		~CLASS() = default; \
	\
	private: \
		static CLASS& GetInstance() \
		{ \
			static CLASS instance; \
			return instance; \
		} \
	public: \
		static const CLASS& GetInstanceRead() { return GetInstance(); } \
		static CLASS& GetInstanceWrite() { return GetInstance(); } \
	private:

#define ENGINE_ASSERT(CONDITION, FORMAT, ...) \
	if (!(CONDITION)) \
	{ \
		wchar_t buffer[1024] = { '\0' }; \
		swprintf(buffer, ARRAYSIZE(buffer), TEXT(FORMAT), __VA_ARGS__); \
		MessageBox(nullptr, buffer, TEXT("ERROR"), MB_OK); \
		assert(false); \
		exit(EXIT_FAILURE); \
	}

#define ENGINE_ASSERT_HRESULT(RESULT) \
	if (FAILED(RESULT)) \
	{ \
		_com_error error(RESULT); \
		LPCTSTR errorMessage = error.ErrorMessage(); \
		wchar_t buffer[1024] = { TEXT('\0') }; \
		swprintf(buffer, ARRAYSIZE(buffer), TEXT("%s"), errorMessage); \
		MessageBox(nullptr, buffer, TEXT("ERROR"), MB_OK); \
		assert(false); \
		exit(EXIT_FAILURE); \
	}

#define ENGINE_ASSERT_HRESULT_SHADER(RESULT, ERROR_BLOB) \
	if (FAILED(RESULT)) \
	{ \
		_com_error error(RESULT); \
		LPCTSTR errorMessage = error.ErrorMessage(); \
		wchar_t errorBlubBuffer[512] = { TEXT('\0') }; \
		wchar_t messageBuffer[1024] = { TEXT('\0') }; \
		{ \
			swprintf(messageBuffer, ARRAYSIZE(messageBuffer), TEXT("%s"), errorMessage); \
		} \
		\
		MessageBox(nullptr, messageBuffer, TEXT("ERROR"), MB_OK); \
		assert(false); \
		exit(EXIT_FAILURE); \
	}

