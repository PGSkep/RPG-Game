#include "Gameplay.h"

#include "CommandBuffer.h"
#include "Console.h"
#include "Renderer.h"
#include "Timer.h"

#include "Input.h"

#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h>  

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetBreakAlloc(-1);

	Timer timer;
	timer.SetResolution(Timer::RESOLUTION_NANOSECONDS);

	Input::Init();
	Input::Reset();

	Renderer renderer;
	renderer.Init();
	
	timer.Play();
	double currentTime = timer.GetTime();
	double lastTime = currentTime;
	while (Input::GetState(Input::ESC) != Input::PRESSED)
	{
		currentTime = timer.GetTime();

		Input::Update();

		double deltaTime = currentTime - lastTime;
		renderer.Update((float)deltaTime);

		lastTime = currentTime;
	}
	
	renderer.ShutDown();
	Input::ShutDown();

	return 0;

	Input::Init();
	Input::Reset();
	Input::Update();

	Con::SetFontSize(5, 5);
	Con::SetSize(1045, 810);

	RegionMap map;
	map.hexMap.Init(25, 25);

	Gameplay gameplay;
	gameplay.Init(&map);

	while (Input::GetState(Input::ESC) != Input::PRESSED)
	{
		Input::Update();

		gameplay.Update();
		gameplay.Render();
	}

	gameplay.ShutDown();
	map.hexMap.ShutDown();

	Input::ShutDown();

	return 0;
}