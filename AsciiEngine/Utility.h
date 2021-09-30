
#ifndef UTILITY_h
#define UTILITY_H

#include <Windows.h>
#include <string>
#include <iostream>
#include <fstream>
#include "Constants.h"
#include "GameObjects.h"
#include <sstream>
#include <algorithm>
//included to help the score from constantly changing all the time
Player player;
Game game;
Background background;

//left this here as if i put it in game.h it would change the array back to 0,0,0,0,0 when exiting the function
int topFiveScores[5];
bool playerHasHitBottom = false;
bool openShop = false;

static int ClampInt(int intToClamp, int lowerLimit, int upperLimit)
{
	if (intToClamp <= lowerLimit)
	{
		return lowerLimit;
	}
	else if (intToClamp >= upperLimit)
	{
		return upperLimit;
	}
	else
	{
		return intToClamp;
	}
}

static float ScoreCalc(const char* backgroundchars,int XPos,int YPos,int HEIGHT,float playerScore,float fuel,float time,bool scored)
{
	//round time to the nearest whole number
	float nearestTime = roundf(time * 100) / 100;
	//gets the multiplyer under the landing zone
	for (int i = -MAX_SIZE_LANDING_PAD; i <= MAX_SIZE_LANDING_PAD; ++i)
	{
		char currentChar = backgroundchars[((background.backgroundYPos * BACKGROUND_ROW_LENGTH) + (XPos + i) + SCREEN_WIDTH * (YPos + HEIGHT))];
		if (currentChar != ' ')
		{
			if (currentChar != 'x' && currentChar != '\\' && currentChar != '/')
			{
				std::stringstream str;
				str << currentChar; int number; str >> number;
				//calculates the player score by (fuel x difficulty(1 = easy, 2x = normal, 3x = hard) x landing area number) / time
				playerScore += (fuel *player.difficulty *(number)+1) / nearestTime;
				game.alreadyScored = true;
				return playerScore;
			}
		}
		
	}
	//if no changes where made return the original player score
	return playerScore;
}

static void LandedOnShop(const char* backgroundchars, int XPos, int YPos, int HEIGHT)
{
	for (int i = -MAX_SIZE_LANDING_PAD; i <= MAX_SIZE_LANDING_PAD; ++i)
	{
		char currentChar = backgroundchars[((background.backgroundYPos * BACKGROUND_ROW_LENGTH) + (XPos + i) + SCREEN_WIDTH * (YPos + HEIGHT))];
		if (currentChar != ' ')
		{
			if (currentChar != 'x' && currentChar != '\\' && currentChar != '/' && (currentChar == 'S' || currentChar == 'H' || currentChar == 'O' || currentChar == 'P'))
			{
				openShop = true;
			}
		}

	}
}

static void ReorganiseFile()
{
	std::string line;
	float numberHolder[NUMBER_HOLDER_MAX];
	int count = 0;
	int tempNumber;
	int comparedNumber;
	std::ifstream file("PlayerScores.txt");
	//fills numberHolder with a value of 0
	for (int i = 0; i < NUMBER_HOLDER_MAX; i++)
	{
		numberHolder[i] = 0;
	}
	//checks if file is open
	if (file && file.is_open())
	{
		//loops until it hits the end of the file
		while (!file.eof())
		{
			//gets all scores and stores them in an array
			std::getline(file, line);
			try
			{
				if (line != "")
				{
					//turns the string value in txt file into a float
					numberHolder[count] = std::stoi(line);
				}
			}
			catch (int n)
			{
				//carry on just to catch the end of the the line
			}
			if (numberHolder[count] < 0)
			{
				//cant have negative score
				numberHolder[count] = 0;
			}
			count++;
		}
		//sorts our scores from lowest to highest
		std::sort(numberHolder, numberHolder + count);
		file.close();

		//this clears the file of all scores
		std::ofstream clearfile("PlayerScores.txt", std::ofstream::out | std::ofstream::trunc);
		clearfile.close();
		
		//opens file again to put scores in from highest to lowest
		std::ofstream writefile("PlayerScores.txt", std::ios::app);
		for (int i = count-1; i >= 0 ; i--)
		{
			if (i != 0)
			{
				if (game.maxTopFiveSlots < 6)
				{
					topFiveScores[game.maxTopFiveSlots] = numberHolder[i];
					game.maxTopFiveSlots++;
				}
				//redoes the order from highest to lowest
				writefile << numberHolder[i];
				writefile << "\n";
			}
			else
			{
				writefile << numberHolder[i];
				writefile.close();
			}
		}
		game.fileExists = true;
		
		//this function was utter pain

	}
	
}

static float ClampFloat(float floatToClamp, float lowerLimit, float upperLimit)
{
	if (floatToClamp <= lowerLimit)
	{
		return lowerLimit;
	}
	else if (floatToClamp >= upperLimit)
	{
		return upperLimit;
	}
	else
	{
		return floatToClamp;
	}
}

static void WriteImageToBuffer(CHAR_INFO* consoleBuffer, const char* charsToPrint, const int coloursToPrint[], const int imageHeight, const int imageWidth, int imageXPos, int imageYPos, int backgroundYPos)
{
	for (int y = 0; y < imageHeight; ++y)
	{
		for (int x = 0; x < imageWidth; ++x)
		{
			consoleBuffer[(imageXPos + x) + SCREEN_WIDTH * (imageYPos + y)].Char.AsciiChar = charsToPrint[x + imageWidth * y];
			if (coloursToPrint)
			{
				consoleBuffer[(imageXPos + x) + SCREEN_WIDTH * (imageYPos + y)].Attributes = coloursToPrint[x + imageWidth * y];
			}
			else
			{
				consoleBuffer[(imageXPos + x) + SCREEN_WIDTH * (imageYPos + y)].Attributes = COLOUR;
			}

		}
	}
}

static void WriteBackgroundToBuffer(CHAR_INFO* consoleBuffer, const char* charsToPrint, const int coloursToPrint[], const int imageHeight, const int imageWidth, int imageXPos, int imageYPos, int YPos)
{
	for (int y = 0; y < imageHeight; ++y)
	{
		for (int x = 0; x < imageWidth; ++x)
		{
			consoleBuffer[(imageXPos + x) + SCREEN_WIDTH * (imageYPos + y)].Char.AsciiChar = charsToPrint[((YPos * BACKGROUND_ROW_LENGTH) + x) + imageWidth * y];
			if (coloursToPrint)
			{
				consoleBuffer[(imageXPos + x) + SCREEN_WIDTH * (imageYPos + y)].Attributes = coloursToPrint[x + imageWidth * y];
			}
			else
			{
				consoleBuffer[(imageXPos + x) + SCREEN_WIDTH * (imageYPos + y)].Attributes = COLOUR;
			}

		}
	}
}

static void WriteSplashToBuffer(CHAR_INFO* consoleBuffer, const int charsToPrint[], const int coloursToPrint[], const int imageHeight, const int imageWidth, int imageXPos, int imageYPos)
{
	for (int y = 0; y < imageHeight; ++y)
	{
		for (int x = 0; x < imageWidth; ++x)
		{
			consoleBuffer[(imageXPos + x) + SCREEN_WIDTH * (imageYPos + y)].Char.AsciiChar = charsToPrint[x + imageWidth * y];
			if (coloursToPrint)
			{
				consoleBuffer[(imageXPos + x) + SCREEN_WIDTH * (imageYPos + y)].Attributes = coloursToPrint[x + imageWidth * y];
			}
			else
			{
				consoleBuffer[(imageXPos + x) + SCREEN_WIDTH * (imageYPos + y)].Attributes = COLOUR;
			}

		}
	}
}

static void ClearScreen(CHAR_INFO* consoleBuffer)
{
	for (int i = 0; i < (SCREEN_WIDTH * SCREEN_HEIGHT); ++i)
	{
		consoleBuffer[i].Char.AsciiChar = 0;
		consoleBuffer[i].Attributes = 0;
	}
}

static void WriteTextToBuffer(CHAR_INFO* consoleBuffer, std::string stringToPrint, int textXPos, int textYPos)
{
	for (int x = 0; x < stringToPrint.length(); ++x)
	{
		consoleBuffer[(textXPos + x) + SCREEN_WIDTH * textYPos].Char.AsciiChar = stringToPrint[x];
		consoleBuffer[(textXPos + x) + SCREEN_WIDTH * textYPos].Attributes = 0xF;
	}
}

#endif //!UTiLITY_H
