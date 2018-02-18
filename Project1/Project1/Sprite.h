#ifndef	SPRITE_H
#define SPRITE_H

#include "Console.h"

void RenderHuman(uint32_t x, uint32_t y, uint8_t color)
{
	uint32_t posX = (uint32_t)x * 8;
	uint32_t posY = (uint32_t)y * 6 + 3 * (x & 1);
	Con::SetColor(color, Con::Intensity::HIGH, Con::Color::BLACK, Con::Intensity::LOW);
	Con::SetCursorPosition(posX + 5, posY + 0);
	std::cout << (char)219;
	Con::SetCursorPosition(posX + 4, posY + 1);
	std::cout << (char)219 << (char)219 << (char)219;
	Con::SetCursorPosition(posX + 5, posY + 2);
	std::cout << (char)219;
	Con::SetCursorPosition(posX + 5, posY + 3);
	std::cout << (char)219;
	Con::SetCursorPosition(posX + 4, posY + 4);
	std::cout << (char)219;
	Con::SetCursorPosition(posX + 6, posY + 4);
	std::cout << (char)219;
}

class Sprite
{
	uint8_t color = Con::Color::WHITE;
	void(*design)(uint32_t x, uint32_t y, uint8_t color);

public:
	static inline Sprite GetSprite(uint8_t _color, void(*_design)(uint32_t x, uint32_t y, uint8_t color))
	{
		Sprite sprite;
		sprite.color = _color;
		sprite.design = _design;
		return sprite;
	}

	void Render(uint32_t x, uint32_t y)
	{
		design(x, y, color);
	}
};

#endif