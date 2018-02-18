#ifndef	GAMEPLAY_H
#define GAMEPLAY_H

#include "Input.h"
#include "CommandBuffer.h"
#include "cmd.h"
#include "RegionMap.h"

class Gameplay
{
public:
	Pawn* player;
	ListS<Pawn> pawns;
	RegionMap* regionMap;

	CommandBuffer commandBuffer;

	void Init(RegionMap* _regionMap)
	{
		commandBuffer.Reserve(1024);
		commandBuffer.Reset();

		pawns.PushBack(Pawn::GetPawn(Sprite::GetSprite(Con::Color::RED, RenderHuman), 10, 10));

		player = &pawns.GetTail().node->data;

		regionMap = _regionMap;
	}

	void Update()
	{
		//if (Input::IsDown(Input::Q))
		//	player->commands.PushFront(Command::GetCommand(cmd::Move, commandBuffer.Allocate(sizeof(cmd::CmdMoveData), &cmd::CmdMoveData::Get(player, player->x - 1, player->y - (~player->x & 1), regionMap))));
		//if (Input::IsDown(Input::W))
		//	player->commands.PushFront(Command::GetCommand(cmd::Move, commandBuffer.Allocate(sizeof(cmd::CmdMoveData), &cmd::CmdMoveData::Get(player, player->x, player->y - 1, regionMap))));
		//if (Input::IsDown(Input::E))
		//	player->commands.PushFront(Command::GetCommand(cmd::Move, commandBuffer.Allocate(sizeof(cmd::CmdMoveData), &cmd::CmdMoveData::Get(player, player->x + 1, player->y - (~player->x & 1), regionMap))));
		//if (Input::IsDown(Input::A))
		//	player->commands.PushFront(Command::GetCommand(cmd::Move, commandBuffer.Allocate(sizeof(cmd::CmdMoveData), &cmd::CmdMoveData::Get(player, player->x - 1, player->y + (player->x & 1), regionMap))));
		//if (Input::IsDown(Input::S))
		//	player->commands.PushFront(Command::GetCommand(cmd::Move, commandBuffer.Allocate(sizeof(cmd::CmdMoveData), &cmd::CmdMoveData::Get(player, player->x, player->y + 1, regionMap))));
		//if (Input::IsDown(Input::D))
		//	player->commands.PushFront(Command::GetCommand(cmd::Move, commandBuffer.Allocate(sizeof(cmd::CmdMoveData), &cmd::CmdMoveData::Get(player, player->x + 1, player->y + (player->x & 1), regionMap))));
		//
		//for (ListS<Pawn>::Iterator iterPawn = pawns.GetHead(); iterPawn.node != nullptr; iterPawn.Next())
		//{
		//	for (ListS<Command>::Iterator iter = iterPawn.node->data.commands.GetHead(); iter.node != nullptr; iter.Next())
		//	{
		//		iter.node->data.cmd(iter.node->data.commandData);
		//	}
		//}

		commandBuffer.Reset();
	}

	void Render()
	{
		for (size_t x = 0; x != regionMap->hexMap.mapWidth; ++x)
		{
			
			for (size_t y = 0; y != regionMap->hexMap.mapHeight; ++y)
			{
				uint32_t posX = (uint32_t)x * 8;
				uint32_t posY = (uint32_t)y * 6 + 3 * (x & 1);
				Con::SetColor(regionMap->hexMap.hex3Map[y*regionMap->hexMap.mapWidth + x].tile.tileID, Con::Intensity::HIGH, Con::Color::BLACK, Con::Intensity::LOW);
				Con::SetCursorPosition(2 + posX, 0 + posY);
				std::cout << (char)219 << (char)219 << (char)219 << (char)219 << (char)219 << (char)219;
				Con::SetCursorPosition(1 + posX, 1 + posY);
				std::cout << (char)219 << (char)219 << (char)219 << (char)219 << (char)219 << (char)219 << (char)219 << (char)219;
				Con::SetCursorPosition(0 + posX, 2 + posY);
				std::cout << (char)219 << (char)219 << (char)219 << (char)219 << (char)219 << (char)219 << (char)219 << (char)219 << (char)219 << (char)219;
				Con::SetCursorPosition(0 + posX, 3 + posY);
				std::cout << (char)219 << (char)219 << (char)219 << (char)219 << (char)219 << (char)219 << (char)219 << (char)219 << (char)219 << (char)219;
				Con::SetCursorPosition(1 + posX, 4 + posY);
				std::cout << (char)219 << (char)219 << (char)219 << (char)219 << (char)219 << (char)219 << (char)219 << (char)219;
				Con::SetCursorPosition(2 + posX, 5 + posY);
				std::cout << (char)219 << (char)219 << (char)219 << (char)219 << (char)219 << (char)219;
			}
		}

		ListS<Pawn>::Iterator iter = pawns.GetHead();

		for (ListS<Pawn>::Iterator iter = pawns.GetHead(); iter.node != nullptr; iter.Next())
		{
			iter.node->data.sprite.Render(iter.node->data.x, iter.node->data.y);
		}
	}

	void ShutDown()
	{
		pawns.Clear();
	}
};

#endif