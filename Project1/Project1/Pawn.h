#ifndef	PAWN_H
#define PAWN_H

#include "Sprite.h"

class Pawn
{
public:
	Sprite sprite;
	uint32_t x, y;

	static Pawn GetPawn(Sprite _sprite, uint32_t _x, uint32_t _y)
	{
		Pawn pawn;
		pawn.sprite = _sprite;
		pawn.x = _x;
		pawn.y = _y;
		return pawn;
	}

	
};

#endif