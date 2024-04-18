#pragma once
#include "PCH.h"

#define NO_COPY(CLASS) \
	CLASS(const CLASS&) = delete; \
	CLASS& operator=(const CLASS&) = delete

#define NO_MOVE(CLASS) \
	CLASS(CLASS&&) = delete; \
	CLASS& operator=(CLASS&&) = delete; \

#define SINGLETON(CLASS) \
	NO_COPY(CLASS); \
	NO_MOVE(CLASS); \
	private: \
		CLASS() = default; \
		~CLASS() = default; \
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

#define ENGINE_ASSERT_W(CONDITION, FORMAT, ...) \
	if (!(CONDITION)) \
	{ \
		wchar_t buffer[1024] = { '\0' }; \
		swprintf(buffer, ARRAYSIZE(buffer), TEXT(FORMAT), __VA_ARGS__); \
		MessageBox(nullptr, buffer, TEXT("ERROR"), MB_OK); \
		assert(false); \
		exit(EXIT_FAILURE); \
	}

#define ENGINE_ASSERT_A(CONDITION, FORMAT, ...) \
	if (!(CONDITION)) \
	{ \
		char buffer[1024] = { '\0' }; \
		__pragma(warning(disable : 4996)) \
		__pragma(warning(push)) \
		sprintf(buffer, FORMAT, __VA_ARGS__); \
		__pragma(warning(pop)) \
		MessageBoxA(nullptr, buffer, "ERROR", MB_OK); \
		assert(false); \
		exit(EXIT_FAILURE); \
	}

#define ENGINE_ASSERT_HRESULT(RESULT) \
	if (FAILED(RESULT)) \
	{ \
		_com_error error(RESULT); \
		LPCTSTR errorMessage = error.ErrorMessage(); \
		\
		wchar_t buffer[1024] = { TEXT('\0') }; \
		swprintf(buffer, ARRAYSIZE(buffer), TEXT("%s"), errorMessage); \
		MessageBox(nullptr, buffer, TEXT("ERROR"), MB_OK); \
		\
		assert(false); \
		exit(EXIT_FAILURE); \
	}

#define ENGINE_ASSERT_HRESULT_SHADER(RESULT, ERROR_BLOB) \
	if (FAILED(RESULT)) \
	{ \
		_com_error error(RESULT); \
		LPCTSTR errorMessage = error.ErrorMessage(); \
		\
		wchar_t errorBlubBuffer[512] = { TEXT('\0') }; \
		wchar_t messageBuffer[1024] = { TEXT('\0') }; \
		\
		if (ERROR_BLOB && ERROR_BLOB->GetBufferPointer() && ERROR_BLOB->GetBufferSize() > 0) \
		{ \
			__pragma(warning(disable : 4996)) \
			__pragma(warning(push)) \
			mbstowcs(errorBlubBuffer, (const char*)ERROR_BLOB->GetBufferPointer(), ERROR_BLOB->GetBufferSize()); \
			__pragma(warning(pop)) \
			swprintf(messageBuffer, ARRAYSIZE(messageBuffer), TEXT("%s\n%s"), errorMessage, errorBlubBuffer); \
		} \
		else \
		{ \
			swprintf(messageBuffer, ARRAYSIZE(messageBuffer), TEXT("%s"), errorMessage); \
		} \
		\
		MessageBox(nullptr, messageBuffer, TEXT("ERROR"), MB_OK); \
		assert(false); \
		exit(EXIT_FAILURE); \
	}\

#define GUID_IS_ZERO(MEMBER) \
	(MEMBER.Data1 == 0 \
	&& MEMBER.Data2 == 0 \
	&& MEMBER.Data3 == 0 \
	&& MEMBER.Data4[0] == 0 \
	&& MEMBER.Data4[1] == 0 \
	&& MEMBER.Data4[2] == 0 \
	&& MEMBER.Data4[3] == 0 \
	&& MEMBER.Data4[4] == 0 \
	&& MEMBER.Data4[5] == 0 \
	&& MEMBER.Data4[6] == 0 \
	&& MEMBER.Data4[7] == 0)

#define ENGINE_CLAMP_F(VALUE, MIN, MAX) \
	fmax(MIN, fmin(VALUE, MAX))

#if defined(ENVIRONMENT32)

#define GET_TICKS() \
__pragma(warning(disable : 28159)) \
__pragma(warning(push)) \
GetTickCount() \
__pragma(warning(pop))

#elif defined(ENVIRONMENT64)

#define GET_TICKS() GetTickCount64(); // In milliseconds.

#else

#error "Define ENVIRONMENT32 or ENVIRONMENT64."

#endif

