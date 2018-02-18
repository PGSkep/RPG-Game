#ifndef	INPUT_H
#define INPUT_H

#include <Windows.h>
#include <vector>
#include <iostream>
#include <string>

namespace Input
{
	enum keys
	{
		ESC							= 1,
		KEY_1						= 2,
		KEY_2						= 3,
		KEY_3						= 4,
		KEY_4						= 5,
		KEY_5						= 6,
		KEY_6						= 7,
		KEY_7						= 8,
		KEY_8						= 9,
		KEY_9						= 10,
		KEY_0						= 11,
		UNDERSCORE					= 12,
		EQUAL						= 13,
		BACKSPACE					= 14,
		TAB							= 15,
		Q							= 16,
		W							= 17,
		E							= 18,
		R							= 19,
		T							= 20,
		Y							= 21,
		U							= 22,
		I							= 23,
		O							= 24,
		P							= 25,
		OPEN_BRACKET				= 26,
		CLOSE_BRACKET				= 27,
		ENTER						= 28,
		CONTROL_LEFT				= 29,
		A							= 30,
		S							= 31,
		D							= 32,
		F							= 33,
		G							= 34,
		H							= 35,
		J							= 36,
		K							= 37,
		L							= 38,
		SEMICOLON					= 39,
		QUOTE						= 40,
		TILDE						= 41,
		SHIFT_LEFT					= 42,
		SLASH						= 43,
		Z							= 44,
		X							= 45,
		C							= 46,
		V							= 47,
		B							= 48,
		N							= 49,
		M							= 50,
		COMMA						= 51,
		PERIOD						= 52,
		QUESTION_MARK				= 53,
		SHIFT_RIGHT					= 54,
		PAD_MULT					= 55,
		ALT_LEFT					= 56,
		SPACE						= 57,
		CAPS						= 58,
		F1							= 59,
		F2							= 60,
		F3							= 61,
		F4							= 62,
		F5							= 63,
		F6							= 64,
		F7							= 65,
		F8							= 66,
		F9							= 67,
		F10							= 68,
		PAUSE						= 69,
		SCROLL_LOCK					= 70,
		PAD_7						= 71,
		PAD_8						= 72,
		PAD_9						= 73,
		PAD_MINUS					= 74,
		PAD_4						= 75,
		PAD_5						= 76,
		PAD_6						= 77,
		PAD_PLUS					= 78,
		PAD_1						= 79,
		PAD_2						= 80,
		PAD_3						= 81,
		PAD_0						= 82,
		PAD_DEL						= 83,
		PAD_ENTER					= 84,
		CONTROL_RIGHT				= 85,
		NUM_LOCK					= 86,
		F11							= 87,
		F12							= 88,
		MOUSE_LEFT					= 89, // mouse
		MOUSE_MID					= 90, // mouse
		MOUSE_RIGHT					= 91, // mouse
		MOUSE_DELTA_X_S16			= 92, // 93 mouse
		MOUSE_DELTA_Y_S16			= 94, // 95 mouse
		MOUSE_WHEEL_DELTA_UP_U16	= 96, // 97 mouse
		MOUSE_WHEEL_DELTA_DOWN_U16	= 98, // 99 mouse
		MOUSE_X_S16					= 100,// 101 mouse
		MOUSE_Y_S16					= 102,// 103 mouse

		PAD_DIVIDE					= 109,

		PRINT_SCREEN				= 111,
		ALT_RIGHT					= 112,

		HOME						= 127,
		ARROW_UP					= 128,
		PAGE_UP						= 129,

		ARROW_LEFT					= 131,

		ARROW_RIGHT					= 133,

		END							= 135,
		ARROW_DOWN					= 136,
		PAGE_DOWN					= 137,
		INSERT						= 138,
		DEL							= 139,

		OS_LEFT						= 147,
		OS_RIGHT					= 148,
		APP							= 149,
	};

	enum state : uint8_t
	{
		PRESSED		= 8,
		HELD		= 12,
		RELEASED	= 2,
		IDLE		= 3,
	};

	HWND hWnd = NULL;
	HINSTANCE hInstance = NULL;
	const char* name = "Input Window";
	const uint8_t dataSize = 149;
	state data[dataSize];

	// After Update1, when handling msg WM_INPUT
	void UpdateData(LPARAM _lParam)
	{
		static char buffer[sizeof(RAWINPUT)] = {};
		static UINT size = sizeof(RAWINPUT);
		GetRawInputData((HRAWINPUT)_lParam, RID_INPUT, buffer, &size, sizeof(RAWINPUTHEADER));

		RAWINPUT* raw = (RAWINPUT*)buffer;
		HANDLE deviceHandle = raw->header.hDevice;
		if (raw->header.dwType == RIM_TYPEKEYBOARD)
		{
			const RAWKEYBOARD& rawKB = raw->data.keyboard;

			UINT flags = rawKB.Flags;
			UINT scanCode = rawKB.MakeCode;
			UINT virtualKey = rawKB.VKey;

			switch (virtualKey)
			{
			case 255:
				return;
			case VK_SHIFT:
				virtualKey = MapVirtualKey(scanCode, MAPVK_VSC_TO_VK_EX);
				break;
			case VK_NUMLOCK:
				scanCode = (MapVirtualKey(virtualKey, MAPVK_VK_TO_VSC) | 0x100);
			}

			const bool isE0 = ((flags & RI_KEY_E0) != 0);
			const bool isE1 = ((flags & RI_KEY_E1) != 0);

			if (isE1)
			{
				if (virtualKey == VK_PAUSE)
					scanCode = 0x45;
				else
					scanCode = MapVirtualKey(virtualKey, MAPVK_VK_TO_VSC);
			}

			const bool isUp = ((flags & RI_KEY_BREAK) != 0);

			UINT res = (scanCode + isE0 * 56);
			if (res == 325)
				res = 86;

			keys key = (keys)res;

			if (isUp)
				data[key] = RELEASED;
			else
				data[key] = PRESSED;

			//UINT key2 = (scanCode << 16) | (isE0 << 24);
			//char buffer2[512] = {};
			//GetKeyNameText((LONG)key2, buffer2, 512);
			//
			//if (isUp)
			//	std::cout << buffer2 << " = " << res << ",\n";
		}
		else if (raw->header.dwType == RIM_TYPEMOUSE)
		{
			const RAWMOUSE& rawM = raw->data.mouse;
			USHORT flags = rawM.usButtonFlags;
			short wheelDelta = (short)rawM.usButtonData;
			LONG x = rawM.lLastX;
			LONG y = rawM.lLastY;

			*((int16_t*)&data[MOUSE_DELTA_X_S16]) += (int16_t)x;
			*((int16_t*)&data[MOUSE_DELTA_Y_S16]) += (int16_t)y;

			switch (flags)
			{
				// left
			case 1:
				data[MOUSE_LEFT] = state::PRESSED;
				break;
			case 2:
				data[MOUSE_LEFT] = state::RELEASED;
				break;
				// right
			case 4:
				data[MOUSE_RIGHT] = state::PRESSED;
				break;
			case 8:
				data[MOUSE_RIGHT] = state::RELEASED;
				break;
				// middle
			case 16:
				data[MOUSE_MID] = state::PRESSED;
				break;
			case 32:
				data[MOUSE_MID] = state::RELEASED;
				break;
				// wheel
			case 1024:
				if (wheelDelta > 0)
					++*((int16_t*)&data[MOUSE_WHEEL_DELTA_UP_U16]);
				else
					++*((int16_t*)&data[MOUSE_WHEEL_DELTA_DOWN_U16]);
				break;
			}

			//std::cout << "	Flags=" << flags << "	WheelDelta=" << wheelDelta << "	X=" << x << "	Y=" << y << "\n";
		}
	}

	// Reference onto how to use UpdateData(LPARAM)
	LRESULT WINAPI MainWndProc(HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam)
	{
		switch(_uMsg)
		{
			case WM_INPUT:
				UpdateData(_lParam);
				break;
		}

		return DefWindowProc(_hWnd, _uMsg, _wParam, _lParam);
	}

	// Must be called once before any updateN()
	static void Reset()
	{
		memset(data, IDLE, sizeof(state) * dataSize);
	}

	// Reference onto how to create a window for input
	static void Init()
	{
		hInstance = GetModuleHandle(NULL);

		WNDCLASSEX winClassEx;
		winClassEx.cbSize = sizeof(WNDCLASSEX);
		winClassEx.style = CS_HREDRAW | CS_VREDRAW;
		winClassEx.lpfnWndProc = MainWndProc;
		winClassEx.cbClsExtra = 0;
		winClassEx.cbWndExtra = 0;
		winClassEx.hInstance = hInstance;
		winClassEx.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		winClassEx.hCursor = LoadCursor(NULL, IDC_ARROW);
		winClassEx.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		winClassEx.lpszMenuName = NULL;
		winClassEx.lpszClassName = name;
		winClassEx.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

		if (!RegisterClassEx(&winClassEx))
			return;

		hWnd = CreateWindowEx(
			0,
			name,
			name,
			0,
			0, 0,
			0,
			0,
			NULL,
			NULL,
			hInstance,
			NULL);

		RAWINPUTDEVICE keyboardDevice;
		keyboardDevice.usUsagePage = 0x01;
		keyboardDevice.usUsage = 0x06;
		keyboardDevice.dwFlags = RIDEV_NOLEGACY | RIDEV_INPUTSINK;
		keyboardDevice.hwndTarget = hWnd;
		RegisterRawInputDevices(&keyboardDevice, 1, sizeof(RAWINPUTDEVICE));

		RAWINPUTDEVICE mouseDevice;
		mouseDevice.usUsagePage = 0x01;
		mouseDevice.usUsage = 0x02;
		mouseDevice.dwFlags = RIDEV_NOLEGACY | RIDEV_INPUTSINK;
		mouseDevice.hwndTarget = hWnd;
		RegisterRawInputDevices(&mouseDevice, 1, sizeof(RAWINPUTDEVICE));

		if (hWnd == NULL)
		{
			UnregisterClass(name, hInstance);
			return;
		}
	}

	// Call before MSG handling
	static void Update1()
	{
		static const keys keyArr[]
		{
			ESC,	KEY_1,	KEY_2,	KEY_3,	KEY_4,	KEY_5,	KEY_6,	KEY_7,	KEY_8,	KEY_9,	KEY_0,	UNDERSCORE,	EQUAL,	BACKSPACE,	TAB,	Q,	W,	E,	R,	T,	Y,	U,	I,	O,	P,
			OPEN_BRACKET,	CLOSE_BRACKET,	ENTER,	CONTROL_LEFT,	A,	S,	D,	F,	G,	H,	J,	K,	L,	SEMICOLON,	QUOTE,	TILDE,	SHIFT_LEFT,	SLASH,	Z,	X,	C,	V,	B,	N,	M,
			COMMA,	PERIOD,	QUESTION_MARK,	SHIFT_RIGHT,	PAD_MULT,	ALT_LEFT,	SPACE,	CAPS,	F1,	F2,	F3,	F4,	F5,	F6,	F7,	F8,	F9,	F10,	PAUSE,	SCROLL_LOCK,	PAD_7,	PAD_8,
			PAD_9,	PAD_MINUS,	PAD_4,	PAD_5,	PAD_6,	PAD_PLUS,	PAD_1,	PAD_2,	PAD_3,	PAD_0,	PAD_DEL,	PAD_ENTER,	CONTROL_RIGHT,	NUM_LOCK,	F11,	F12,	MOUSE_LEFT,
			MOUSE_MID,	MOUSE_RIGHT,	PAD_DIVIDE,	PRINT_SCREEN,	ALT_RIGHT,	HOME,	ARROW_UP,	PAGE_UP,	ARROW_LEFT,	ARROW_RIGHT,	END,	ARROW_DOWN,	PAGE_DOWN,	INSERT,
			DEL,	OS_LEFT,	OS_RIGHT,	APP,
		};

		for (size_t i = 0; i != sizeof(keyArr) / sizeof(keys); ++i)
		{
			if (data[keyArr[i]] == state::PRESSED)
				data[keyArr[i]] = state::HELD;
			else if (data[keyArr[i]] == state::RELEASED)
				data[keyArr[i]] = state::IDLE;
		}

		static const keys keyArr2[] = 
		{
			MOUSE_DELTA_X_S16,	MOUSE_DELTA_Y_S16,
			MOUSE_WHEEL_DELTA_UP_U16,	MOUSE_WHEEL_DELTA_DOWN_U16,
		};

		for (size_t i = 0; i != sizeof(keyArr2) / sizeof(keys); ++i)
		{
			*((uint16_t*)&data[keyArr2[i]]) = 0U;
		}
	}

	// MSG handling
	static void Update2()
	{
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	// Call after MSG handling
	static void Update3()
	{
		POINT point;
		GetCursorPos(&point);

		*((int16_t*)&data[MOUSE_X_S16]) = (int16_t)point.x;
		*((int16_t*)&data[MOUSE_Y_S16]) = (int16_t)point.y;
	}

	// Reference for update calls
	static void Update()
	{
		Update1();
		Update2();
		Update3();
	}

	// All keys
	static state GetState(keys _key)
	{
		return data[_key];
	}

	static bool IsUp(keys _key)
	{
		return (data[_key] & IDLE) != 0;
	}
	static bool IsDown(keys _key)
	{
		return (data[_key] & HELD) != 0;
	}
	static bool IsPressed(keys _key)
	{
		return data[_key] == PRESSED;
	}
	static bool IsReleased(keys _key)
	{
		return data[_key] == RELEASED;
	}
	static bool IsIdle(keys _key)
	{
		return data[_key] == IDLE;
	}
	static bool IsHeld(keys _key)
	{
		return data[_key] == HELD;
	}

	// MOUSE_X_S16, MOUSE_Y_S16, MOUSE_DELTA_X_S16, MOUSE_DELTA_Y_S16
	static int16_t GetMouse(keys _key)
	{
		return *((int16_t*)&data[_key]);
	}

	// MOUSE_WHEEL_DELTA_UP_U16, MOUSE_WHEEL_DELTA_DOWN_U16
	static uint16_t GetWheel(keys _key)
	{
		return *((uint16_t*)&data[_key]);
	}

	// Call if Init() was called
	static void ShutDown()
	{
		DestroyWindow(hWnd);
		UnregisterClass(name, hInstance);
	}
}

#endif