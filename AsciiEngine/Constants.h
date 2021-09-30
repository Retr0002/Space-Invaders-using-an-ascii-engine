#ifndef CONSTANTS_H
#define CONSTANTS_H

//headers
#include <Windows.h>

// Defines
#define SCREEN_WIDTH 150
#define SCREEN_HEIGHT 40

//KEYS
const int KEY_ESC = VK_ESCAPE;
const int KEY_ENTER = VK_RETURN;
const int KEY_W = 'W';
const int KEY_A = 'A';
const int KEY_S = 'S';
const int KEY_D = 'D';
const int KEY_1 = '1';
const int KEY_2 = '2';
const int KEY_3 = '3';
const int KEY_4 = '4';
const int KEY_5 = '5';

//player parameters
const float ACCELERATION_RATE = 1.0f;
const float DECELERATION_RATE = 0.2f;
const float FUEL_CONSUMPTION_RATE = 0.5f;
const float JET_FUEL_MODIFIER = 0.3;
const float EFFICIENT_ENGINE_MODIFIER = 0.2;
const float EASY_FUEL = 60;
const float NORMAL_FUEL = 45;
const float HARD_FUEL = 30;

//Utility constants
const int NUMBER_HOLDER_MAX = 1000;
const int BACKGROUND_ROW_LENGTH = 150;
const int COLOUR = 7;
const int BACKGROUND_STARTING_POINT = 62;
const int MAX_SIZE_LANDING_PAD = 15;

#endif // !CONSTANTS_H

