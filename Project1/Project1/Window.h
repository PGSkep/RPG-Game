#ifndef	WINDOW_H
#define WINDOW_H

#include "Error.h"

class Window
{
	std::string name;

public:
	HINSTANCE hInstance = NULL;
	HWND hWnd = NULL;

	std::vector<Error> Init(const char* _name, const char* _title, WNDPROC _wndProc, int _width, int _height)
	{
		std::vector<Error> error;

		name = _name;
		hInstance = GetModuleHandle(NULL);

		WNDCLASSEX winClassEx;
		winClassEx.cbSize = sizeof(WNDCLASSEX);
		winClassEx.style = CS_HREDRAW | CS_VREDRAW;
		winClassEx.lpfnWndProc = _wndProc;
		winClassEx.cbClsExtra = 0;
		winClassEx.cbWndExtra = 0;
		winClassEx.hInstance = hInstance;
		winClassEx.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		winClassEx.hCursor = LoadCursor(NULL, IDC_ARROW);
		winClassEx.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		winClassEx.lpszMenuName = NULL;
		winClassEx.lpszClassName = name.c_str();
		winClassEx.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

		if (!RegisterClassEx(&winClassEx))
		{
			PUSH_ERROR("!RegisterClassEx");
		}

		RECT rect = { 0, 0, 800, 600 };

		DWORD dwStyle = WS_OVERLAPPEDWINDOW;

		AdjustWindowRect(&rect, dwStyle, FALSE);

		hWnd = CreateWindowEx(0,
			name.c_str(),
			_title,
			dwStyle | WS_VISIBLE | WS_SYSMENU,
			0, 0,
			rect.right - rect.left,
			rect.bottom - rect.top,
			NULL,
			NULL,
			hInstance,
			NULL);

		if (hWnd == NULL)
		{
			UnregisterClass(name.c_str(), hInstance);
			PUSH_ERROR("_window.hWnd == NULL");
		}

		ShowWindow(hWnd, SW_SHOW);
		SetForegroundWindow(hWnd);
		SetFocus(hWnd);

		return error;
	}
	void Update()
	{
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	void ShutDown()
	{
		UnregisterClass(name.c_str(), hInstance);
		DestroyWindow(hWnd);
	}
};

#endif