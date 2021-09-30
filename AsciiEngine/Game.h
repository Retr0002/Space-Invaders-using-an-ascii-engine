#ifndef GAME_H
#define GAME_H

// Headers
#include <Windows.h>
#include "GameObjects.h"

class Game
{
public:
	//functions
	Game();
	void Initialise();
	void Update(float deltaTime);
	void Draw();
	

private:
	// ENUMS
	enum GAME_STATE
	{
		SPLASH,
		MENU,
		PLAY,
		OPTIONS,
		SCOREBOARD,
		SHOP,
		

	};
	// console parameters
	// Windows size coords
	SMALL_RECT windowsSize{ 0,0,SCREEN_WIDTH - 1,SCREEN_HEIGHT - 1 };

	// a COORD struct for specifying the consoles screen buffer dimentions
	COORD bufferSize = { SCREEN_WIDTH,SCREEN_HEIGHT };

	//setting up different variables for passing to WriteConsoleOutput
	COORD characterBufferSize = { SCREEN_WIDTH,SCREEN_HEIGHT };
	COORD characterPosition = { 0,0 };
	SMALL_RECT consoleWriteArea = { 0,0,SCREEN_WIDTH - 1,SCREEN_HEIGHT - 1 };

	// a CHAR_INFO structure containing data about our frame
	CHAR_INFO consoleBuffer[SCREEN_WIDTH * SCREEN_HEIGHT];

	// initialise handles
	HANDLE wHnd = GetStdHandle(STD_OUTPUT_HANDLE);
	HANDLE rHnd = GetStdHandle(STD_INPUT_HANDLE);

	//game variables
	GAME_STATE currentGameState = SPLASH;
	Splash splash;
	Background background;
	Player player;
	Explosion explosion;
	Timer timer;

	//variables
public:
	bool exitGame = false;
	float nearestTime = 0.0f;
	bool alreadyScored = false;
	bool writtenToFile = false;
	bool fileExists = false;
	int maxTopFiveSlots = 0;

};
#endif // !GAME_H


