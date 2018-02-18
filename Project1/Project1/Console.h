#ifndef CONSOLE_H
#define CONSOLE_H

#include <Windows.h>
#include <iostream>

namespace Con
{
	struct Color
	{
		enum COLORS
		{
			BLACK = 0,
			BLUE = 1,
			GREEN = 2,
			CYAN = 3,
			RED = 4,
			PINK = 5,
			YELLOW = 6,
			WHITE = 7,
		};
	};

	struct Intensity
	{
		enum INTENSITY
		{
			LOW = 0,
			HIGH = 8
		};
	};

	static void SetColor(short foreground, short fore_intensity, short background, short back_intensity)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (foreground + fore_intensity) + ((background + back_intensity) * 16));
	}

	static void SetPosition(int _x, int _y)
	{
		SetWindowPos(GetConsoleWindow(), 0, _x, _y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	}

	static void SetSize(int _width, int _height)
	{
		SetWindowPos(GetConsoleWindow(), 0, 0, 0, _width, _height, SWP_NOZORDER | SWP_NOMOVE);
	}

	static void SetCursorPosition(SHORT x, SHORT y)
	{
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {x, y});
	}

	static void SetFontSize(SHORT x, SHORT y)
	{
		CONSOLE_FONT_INFOEX info = { 0 };
		info.cbSize = sizeof(info);
		info.dwFontSize = {x, y};
		info.FontWeight = FW_NORMAL;
		wcscpy(info.FaceName, L"Lucida Console");
		SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), NULL, &info);
	}
};

#endif