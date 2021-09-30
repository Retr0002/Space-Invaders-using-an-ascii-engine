//includes
#include <Windows.h>
#include<chrono>
#include<string>
#include "Game.h"
#include "GameObjects.h"




// TypeDefs
typedef std::chrono::steady_clock::time_point Time;
typedef std::chrono::high_resolution_clock HiResClock;
typedef std::chrono::duration<float> TimeDiff;






//Game Variables
unsigned int frameCounter = 0;
Time previousFrameTime = HiResClock::now();



int main()
{
	float deltaTime = 0.0f;
	Game gameInstance;
	Player player;
	// Initialise console window
	gameInstance.Initialise();

	//main game loop
	while (!gameInstance.exitGame)
	{
		
		//Calculate deltaTime
		Time currentFrameTime = HiResClock::now();
		TimeDiff diff = currentFrameTime - previousFrameTime;
		deltaTime = diff.count();
		frameCounter++;

		if (deltaTime >= (1.0f / player.frameRate))
		{
			//updates our application
			gameInstance.Update(deltaTime);

			// Cache TimeStap of frame
			previousFrameTime = currentFrameTime;

			
		}
		gameInstance.Draw();
	}
	
}


