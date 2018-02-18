#ifndef	CMD_H
#define CMD_H

#include "CommandBuffer.h"
#include "Pawn.h"
#include "RegionMap.h"

#include <stdint.h>

namespace cmd
{
	struct CmdMoveData
	{
		Pawn* pawn;
		uint32_t newX, newY;
		RegionMap* regionMap;

		static inline CmdMoveData Get(Pawn* _pawn, uint32_t _newX, uint32_t _newY, RegionMap* _regionMap)
		{
			CmdMoveData cmdMoveData;
			cmdMoveData.pawn = _pawn;
			cmdMoveData.newX = _newX;
			cmdMoveData.newY = _newY;
			cmdMoveData.regionMap = _regionMap;
			return cmdMoveData;
		}
	};
	void Move(void* _data)
	{
		CmdMoveData* data = (CmdMoveData*)_data;

		if (data->newX < data->regionMap->hexMap.mapWidth && data->newX + 1 > 0)
		{
			if (data->newY < data->regionMap->hexMap.mapHeight && data->newY + 1 > 0)
			{
				if (data->regionMap->hexMap.hex3Map[data->regionMap->hexMap.mapWidth * data->newY + data->newX].pawn == nullptr)
					return;

				data->pawn->x = data->newX;
				data->pawn->y = data->newY;
			}
		}
	}
}

#endif