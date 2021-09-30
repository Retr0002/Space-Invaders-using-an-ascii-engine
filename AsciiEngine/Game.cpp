// Header
#include "Game.h"
#include "Utility.h"
#include "Constants.h"
#include <iostream>
#include <fstream>



Game::Game()
{
	// Constructor
}

void Game::Initialise()
{
	//Set Console Title
	SetConsoleTitle(L"Lunar Lander");

	//set screen buffer size
	SetConsoleScreenBufferSize(wHnd, bufferSize);

	//set the windows size	
	SetConsoleWindowInfo(wHnd, TRUE, &windowsSize);
}


void Game::Update(float deltaTime)
{

	switch (currentGameState) 
	{
	case SPLASH:
	{
		//calc current duration
		splash.splashDuration += deltaTime;

		if (splash.splashDuration < 3.0f)
		{
			//Draw Splash Image
			WriteSplashToBuffer(consoleBuffer, splash.CHARACTERS, splash.COLOURS, splash.HEIGHT, splash.WIDTH, (SCREEN_WIDTH / 2) - (splash.WIDTH / 2), (SCREEN_HEIGHT / 2) - (splash.HEIGHT / 2));
		}
		else
		{
			//move state
			splash.splashDuration = 0.0f;
			currentGameState = MENU;
		}

		break;
	}
	case MENU:
	{
		//clear previous screen
		ClearScreen(consoleBuffer);

		//print out text options
		if (player.canContinue)
		{
			WriteTextToBuffer(consoleBuffer, "1.CONTINUE", (SCREEN_WIDTH / 2) - 5, (SCREEN_HEIGHT / 2) - 3);
			WriteTextToBuffer(consoleBuffer, "2.OPTIONS - ALREADY INGAME", (SCREEN_WIDTH / 2) - 5, (SCREEN_HEIGHT / 2) - 2);;
		}
		else
		{
			WriteTextToBuffer(consoleBuffer, "1.PLAY", (SCREEN_WIDTH / 2) - 5, (SCREEN_HEIGHT / 2) - 3);
			WriteTextToBuffer(consoleBuffer, "2.OPTIONS", (SCREEN_WIDTH / 2) - 5, (SCREEN_HEIGHT / 2) - 2);;
			player.playerScore = 0.0f;
		}

		WriteTextToBuffer(consoleBuffer, "3.SCOREBOARD", (SCREEN_WIDTH / 2) - 5, (SCREEN_HEIGHT / 2) - 1);
		WriteTextToBuffer(consoleBuffer, "4.QUIT", (SCREEN_WIDTH / 2) - 5, (SCREEN_HEIGHT / 2));
		if (player.difficulty == 3)
		{
			WriteTextToBuffer(consoleBuffer, "CURRENT DIFFICULTY: HARD", (SCREEN_WIDTH / 2) - 5, (SCREEN_HEIGHT / 2) + 1);
		}
		else if (player.difficulty == 2)
		{
			WriteTextToBuffer(consoleBuffer, "CURRENT DIFFICULTY: NORMAL", (SCREEN_WIDTH / 2) - 5, (SCREEN_HEIGHT / 2) + 1);
		}
		else if (player.difficulty == 1)
		{
			WriteTextToBuffer(consoleBuffer, "CURRENT DIFFICULTY: EASY", (SCREEN_WIDTH / 2) - 5, (SCREEN_HEIGHT / 2) + 1);
		}

		if (GetAsyncKeyState(KEY_1))
		{
			currentGameState = PLAY;
			timer.reset();
			game.alreadyScored = false;
			background.newGame = true;
		}
		if (GetAsyncKeyState(KEY_2) && !player.canContinue)
		{
			currentGameState = OPTIONS;
		}
		if (GetAsyncKeyState(KEY_3))
		{
			currentGameState = SCOREBOARD;
			game.maxTopFiveSlots = 0;
			ReorganiseFile();
		}
		if (GetAsyncKeyState(KEY_4))
		{
			exitGame = true;
		}

		break;
	}
	case PLAY:
	{
		//clear the previous frame before we start next one
		ClearScreen(consoleBuffer);

		//start the timer
		timer.start();

		//get keyinputs
		if (GetAsyncKeyState(KEY_ESC))
		{
			exitGame = true;


		}
		if (GetAsyncKeyState(KEY_ENTER))
		{
			player.Reset();
			game.writtenToFile = false;
			currentGameState = MENU;


		}
		//take the input

		if (!player.hasLanded && !player.hasCrashed)
		{
			if (GetAsyncKeyState(KEY_ESC))
			{
				exitGame = true;
			}
			if (GetAsyncKeyState(KEY_A) && player.fuel > 0)
			{
				--player.XPos;

				//use fuel
				if (player.hasUpgradedEngine)
				{
					player.fuel -= (FUEL_CONSUMPTION_RATE - EFFICIENT_ENGINE_MODIFIER);
				}
				else
				{
					player.fuel -= FUEL_CONSUMPTION_RATE;
				}

				player.aKeyPressed = true;
				
			}
			if (GetAsyncKeyState(KEY_W) && player.fuel > 0)
			{
				player.isAccelerating = true;
				player.wKeyPressed = true;

				// Use Fuel
				if (player.hasUpgradedEngine)
				{
					player.fuel -= (FUEL_CONSUMPTION_RATE - EFFICIENT_ENGINE_MODIFIER);
				}
				else
				{
					player.fuel -= FUEL_CONSUMPTION_RATE;
				}

				

			}
			if (GetAsyncKeyState(KEY_D) && player.fuel > 0)
			{
				++player.XPos;

				//use fuel
				if (player.hasUpgradedEngine)
				{
					player.fuel -= (FUEL_CONSUMPTION_RATE - EFFICIENT_ENGINE_MODIFIER);
				}
				else
				{
					player.fuel -= FUEL_CONSUMPTION_RATE;
				}
				
				player.dKeyPressed = true;
				
			}
			//check for acceleration and apply upgrades
			if (player.isAccelerating && player.hasUpgradedFuel)
			{
				player.Acceleration += ((ACCELERATION_RATE+JET_FUEL_MODIFIER) * deltaTime);
			}
			else if (player.isAccelerating)
			{
				player.Acceleration += (ACCELERATION_RATE * deltaTime);

				//if (player.Acceleration > 0.0)
				//{
					//OutputDebugStringW(L"\nAccelerating" + int(player.Acceleration));
				//}
			}
			else
			{
				player.Acceleration -= (DECELERATION_RATE * deltaTime);
			}

			//reset acceleration flag
			player.isAccelerating = false;

			//clamp acceleration
			player.Acceleration = ClampFloat(player.Acceleration, 0.0f, 2.0f);


			if (player.Acceleration >= player.upperAccelerationLimit && player.YPos <= 2 && background.backgroundYPos !=0)
			{
				playerHasHitBottom = false;
				background.backgroundYPos--;
				
			}
			else if (player.Acceleration >= player.upperAccelerationLimit)
			{
				player.YPos--;
			}
			else if (player.Acceleration < player.lowerAccelerationLimit && playerHasHitBottom)
			{
				player.YPos++;
			}
			else if (player.Acceleration < player.lowerAccelerationLimit && !playerHasHitBottom)
			{
				background.backgroundYPos++;
				if (background.backgroundYPos >= BACKGROUND_STARTING_POINT)
				{
					playerHasHitBottom = true;
				}
			}

		}



		//clamp the input
		player.XPos = ClampInt(player.XPos, 0, (SCREEN_WIDTH - player.WIDTH));
		player.YPos = ClampInt(player.YPos, 0, (SCREEN_HEIGHT - player.HEIGHT));

		//get Two characters under landing gear
		char bottumLeftChar = background.CHARACTERS[(background.backgroundYPos * BACKGROUND_ROW_LENGTH)+((player.XPos + (player.WIDTH - 3)) + SCREEN_WIDTH * (player.YPos + player.HEIGHT - 1))];
		char bottumRightChar = background.CHARACTERS[(background.backgroundYPos * BACKGROUND_ROW_LENGTH)+(player.XPos + (player.WIDTH - 2)) + SCREEN_WIDTH * (player.YPos + (player.HEIGHT - 1))];

		//landed
		if (bottumLeftChar == '_' && bottumRightChar == '_')
		{
			player.hasLanded = true;
		}
		else if (bottumLeftChar != ' ' || bottumRightChar != ' ')
		{
			player.hasCrashed = true;
		}



		//sets hitbottom to true if it hits bottom
		


		//draw background image
		if (!playerHasHitBottom && !background.newGame)
		{
			WriteBackgroundToBuffer(consoleBuffer, background.CHARACTERS, nullptr, SCREEN_HEIGHT, SCREEN_WIDTH, 0, 0, background.backgroundYPos);
		}
		else if (playerHasHitBottom || background.newGame)
		{
			WriteBackgroundToBuffer(consoleBuffer, background.CHARACTERS, nullptr, SCREEN_HEIGHT, SCREEN_WIDTH, 0, 0, BACKGROUND_STARTING_POINT);
			if (background.newGame)
			{
				background.backgroundYPos = BACKGROUND_STARTING_POINT;
				background.newGame = false;
			}
			
			
		}


		//Draw Crashed Player
		if (player.hasCrashed)
		{
			player.hasUpgradedEngine = false;
			player.hasUpgradedFuel = false;
			player.difficulty = 3;
			player.canContinue = false;
			WriteTextToBuffer(consoleBuffer, "HOUSTON, WE HAVE A PROBLEM", (SCREEN_WIDTH / 2)-10, (SCREEN_HEIGHT / 2)-10);
			WriteTextToBuffer(consoleBuffer, "PRESS ENTER TO GO TO MENU OR ESC TO EXIT", (SCREEN_WIDTH / 2) - 17, (SCREEN_HEIGHT / 2) - 9);
			static float explosionFlashTimer = 0.0f;


			explosionFlashTimer += deltaTime;

			if (explosionFlashTimer >= 0.5f)
			{
				//draw explosion
				WriteImageToBuffer(consoleBuffer, explosion.CHARACTERS, explosion.COLOURS, explosion.HEIGHT, explosion.WIDTH, player.XPos, player.YPos,0);
				explosionFlashTimer = 0.0f;
			}
			else
			{
				WriteImageToBuffer(consoleBuffer, explosion.EMPTY_CHARACTERS, explosion.COLOURS, explosion.HEIGHT, explosion.WIDTH, player.XPos, player.YPos,0);
			}
		}
		else if (player.hasLanded)
		{
			WriteTextToBuffer(consoleBuffer, "LANDED!", (SCREEN_WIDTH / 2) -3, (SCREEN_HEIGHT / 2) - 10);
			WriteTextToBuffer(consoleBuffer, "PRESS ENTER TO CARRY ON OR ESC TO EXIT", (SCREEN_WIDTH / 2) - 17, (SCREEN_HEIGHT / 2) - 9);
			//Allow to continue
			player.canContinue = true;
			//Draw Player image
			WriteImageToBuffer(consoleBuffer, player.CHARACTERS_NEUTRAL, player.COLOURS, player.HEIGHT, player.WIDTH, player.XPos, player.YPos,0);
			LandedOnShop(background.CHARACTERS, player.XPos, player.YPos, player.HEIGHT);
		}
		//draws out player character if going left,right,up or neutral
		else if (!player.playerDraw && player.aKeyPressed)
		{
			WriteImageToBuffer(consoleBuffer, player.CHARACTERS_RIGHT, player.COLOURS, player.HEIGHT, player.WIDTH, player.XPos, player.YPos, 0);
			player.playerDraw = true;
			player.aKeyPressed = false;
		}
		else if (!player.playerDraw && player.wKeyPressed)
		{
			WriteImageToBuffer(consoleBuffer, player.CHARACTERS_UP, player.COLOURS, player.HEIGHT, player.WIDTH, player.XPos, player.YPos, 0);
			player.playerDraw = true;
			player.wKeyPressed = false;
		}
		else if (!player.playerDraw && player.dKeyPressed)
		{
			WriteImageToBuffer(consoleBuffer, player.CHARACTERS_LEFT, player.COLOURS, player.HEIGHT, player.WIDTH, player.XPos, player.YPos, 0);
			player.playerDraw = true;
			player.dKeyPressed = false;
		}
		else if (!player.playerDraw)
		{
			WriteImageToBuffer(consoleBuffer, player.CHARACTERS_NEUTRAL, player.COLOURS, player.HEIGHT, player.WIDTH, player.XPos, player.YPos, 0);
		}
		




		//Draw UI text
		if ((player.hasCrashed && !game.alreadyScored) || (player.hasLanded && !game.alreadyScored))
		{
			player.playerScore = ScoreCalc(background.CHARACTERS, player.XPos, player.YPos, player.HEIGHT, player.playerScore, player.fuel, nearestTime, game.alreadyScored);
			if (player.hasCrashed && !game.writtenToFile)
			{
				std::ofstream file;
				file.open("PlayerScores.txt", std::ios::app);
				file << "\n";
				file << player.playerScore;
				file.close();
				game.writtenToFile = true;
			}



		}
		if (!player.hasLanded && !player.hasCrashed && !timer.resetted)
		{
			nearestTime = timer.currentTime();
		}
		WriteTextToBuffer(consoleBuffer, "Score:" + std::to_string(player.playerScore), 1, 0);
		WriteTextToBuffer(consoleBuffer, "Time:" + std::to_string(nearestTime), 1, 1);
		WriteTextToBuffer(consoleBuffer, "Fuel:" + std::to_string(player.fuel), 1, 2);

		player.playerDraw = false;
		if (openShop == true)
		{
			currentGameState = SHOP;
			player.Reset();
		}
		break;
		if (player.hasCrashed)
		{
			player.fuel = HARD_FUEL;
		}
	}
	case OPTIONS:
	{
		ClearScreen(consoleBuffer);
		WriteTextToBuffer(consoleBuffer, "1.HARD", (SCREEN_WIDTH / 2) - 5, (SCREEN_HEIGHT / 2) - 3);
		WriteTextToBuffer(consoleBuffer, "2.NORMAL", (SCREEN_WIDTH / 2) - 5, (SCREEN_HEIGHT / 2) - 2);
		WriteTextToBuffer(consoleBuffer, "3.EASY", (SCREEN_WIDTH / 2) - 5, (SCREEN_HEIGHT / 2) - 1);

		if (GetAsyncKeyState(KEY_1))
		{
			player.difficulty = 3;
			player.frameRate = 15;
			player.fuel = HARD_FUEL;
			currentGameState = MENU;

		}
		if (GetAsyncKeyState(KEY_2))
		{
			player.difficulty = 2;
			player.frameRate = 10;
			player.fuel = NORMAL_FUEL;
			currentGameState = MENU;
		}
		if (GetAsyncKeyState(KEY_3))
		{
			player.difficulty = 1;
			player.frameRate = 5;
			player.fuel = EASY_FUEL;
			currentGameState = MENU;
		}
		break;
	}
	case SCOREBOARD:
	{
		ClearScreen(consoleBuffer);
		if (game.fileExists)
		{
			//writes out the top 5 scores
			WriteTextToBuffer(consoleBuffer, "TOP 5 SCORES:", (SCREEN_WIDTH / 2) - 5, (SCREEN_HEIGHT / 2) - 8);
			WriteTextToBuffer(consoleBuffer, "1." + std::to_string(topFiveScores[0]), (SCREEN_WIDTH / 2) - 5, (SCREEN_HEIGHT / 2) - 5);
			WriteTextToBuffer(consoleBuffer, "2." + std::to_string(topFiveScores[1]), (SCREEN_WIDTH / 2) - 5, (SCREEN_HEIGHT / 2) - 4);
			WriteTextToBuffer(consoleBuffer, "3." + std::to_string(topFiveScores[2]), (SCREEN_WIDTH / 2) - 5, (SCREEN_HEIGHT / 2) - 3);
			WriteTextToBuffer(consoleBuffer, "4." + std::to_string(topFiveScores[3]), (SCREEN_WIDTH / 2) - 5, (SCREEN_HEIGHT / 2) - 2);
			WriteTextToBuffer(consoleBuffer, "5." + std::to_string(topFiveScores[4]), (SCREEN_WIDTH / 2) - 5, (SCREEN_HEIGHT / 2) - 1);
			WriteTextToBuffer(consoleBuffer, "PRESS 1 TO RETURN TO MENU", (SCREEN_WIDTH / 2) - 5, (SCREEN_HEIGHT / 2) + 2);
		}
		else
		{
			WriteTextToBuffer(consoleBuffer, "NO SCORES RECORDED!", (SCREEN_WIDTH / 2) - 5, (SCREEN_HEIGHT / 2) - 3);
			WriteTextToBuffer(consoleBuffer, "PRESS 1 TO RETURN TO MENU", (SCREEN_WIDTH / 2) - 5, (SCREEN_HEIGHT / 2) - 2);
		}
		if (GetAsyncKeyState(KEY_1))
		{
			currentGameState = MENU;
		}
		break;
	}
	case SHOP:
	{
		
		//player.playerScore = 10000.0f;
		
		ClearScreen(consoleBuffer);
		//sets landed on shop to false as we start again
		openShop = false;
		//write text Ui
		WriteTextToBuffer(consoleBuffer, "Score:" + std::to_string(player.playerScore), 1, 0);
		WriteTextToBuffer(consoleBuffer, "Fuel:" + std::to_string(player.fuel), 1, 1);
		WriteTextToBuffer(consoleBuffer, "SHOP", (SCREEN_WIDTH / 2) - 5, (SCREEN_HEIGHT / 2) - 9);
		WriteTextToBuffer(consoleBuffer, "YOU PAY WITH POINTS", (SCREEN_WIDTH / 2) - 10, (SCREEN_HEIGHT / 2) - 8);
		WriteTextToBuffer(consoleBuffer, "1. 10 Fuel - 50 Score", (SCREEN_WIDTH / 2) - 10, (SCREEN_HEIGHT / 2) - 6);
		WriteTextToBuffer(consoleBuffer, "2. 20 Fuel - 90 Score", (SCREEN_WIDTH / 2) - 10, (SCREEN_HEIGHT / 2) - 5);
		WriteTextToBuffer(consoleBuffer, "3. 30 Fuel - 140 Score", (SCREEN_WIDTH / 2) - 10, (SCREEN_HEIGHT / 2) - 4);
		WriteTextToBuffer(consoleBuffer, "PRESS ENTER TO GO BACK TO MENU", (SCREEN_WIDTH / 2) - 10, (SCREEN_HEIGHT / 2) - 1);
		if (!player.hasUpgradedFuel)
		{
			WriteTextToBuffer(consoleBuffer, "4. Jet Fuel - 300 Score !ONE TIME", (SCREEN_WIDTH / 2) - 10, (SCREEN_HEIGHT / 2) - 3);
		}
		if (!player.hasUpgradedEngine)
		{
			WriteTextToBuffer(consoleBuffer, "5. Efficient engines - 500 Score !ONE TIME", (SCREEN_WIDTH / 2) - 10, (SCREEN_HEIGHT / 2) - 2);
		}
		
		//applies purchases
		if (GetAsyncKeyState(KEY_1))
		{
			player.playerScore -= 50;
			if (player.playerScore < 0)
			{
				player.playerScore += 50;
			}
			else
			{
				player.fuel += 10;
			}

		}
		if (GetAsyncKeyState(KEY_2))
		{
			player.playerScore -= 90;
			if (player.playerScore < 0)
			{
				player.playerScore += 90;
			}
			else
			{
				player.fuel += 20;
			}
		}
		if (GetAsyncKeyState(KEY_3))
		{
			player.playerScore -= 140;
			if (player.playerScore < 0)
			{
				player.playerScore += 140;
			}
			else
			{
				player.fuel += 30;
			}
		}
		if (GetAsyncKeyState(KEY_4))
		{
			player.playerScore -= 300;
			if (player.playerScore < 0)
			{
				player.playerScore += 300;
			}
			else
			{
				player.hasUpgradedFuel = true;
			}

		}
		if (GetAsyncKeyState(KEY_5))
		{
			player.playerScore -= 500;
			if (player.playerScore < 0)
			{
				player.playerScore += 500;
			}
			else
			{
				player.hasUpgradedEngine = true;
			}
			
			
		}
		if (GetAsyncKeyState(KEY_ENTER))
		{
			currentGameState = MENU;
		}
		
		
		
		
		
		break;

	}
	}

}


void Game::Draw()
{
	//Kick the draw
	WriteConsoleOutputA(wHnd, consoleBuffer, characterBufferSize, characterPosition, &consoleWriteArea);
}