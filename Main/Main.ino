#include <LiquidCrystal.h>
#include "ButtonDebounce.h"

volatile int encoderPosition = 0;
int numRows= 4;
int numCols= 20;

float curRow;
float curCol;
char contents[4][20];
int lives = 5;

unsigned long timer;
unsigned long startTime;
unsigned	long delta;
LiquidCrystal LcdDriver(11,9,5,6,7,8);

// Set up pin and button state.
int bState;
//enum buttonStates {BS_Idle,BS_Wait,BS_Low};
//buttonStates buttonState = BS_Idle;
enum GameStates { Pause,NewGame, Playing, BetweenGames, EndOfGame };
enum GameNumber {Game1, Game2};
GameNumber gameState = Game1; 
GameStates curState = Playing;
unsigned long bTime;
char emptyChar =' ';
char spriteChar='*';

int INTERVAL = 200;
#define INIT_INTERVAL  200
//ButtonInitialize(4);

void setup()
{
	//pinMode(12,OUTPUT);
	pinMode(4,INPUT);
	prepopulateArray();
	attachInterrupt(digitalPinToInterrupt(2), MonitorA, CHANGE);
	attachInterrupt(digitalPinToInterrupt(3), MonitorB, CHANGE);
	Serial.begin(9600);
	ButtonInitialize(4);
	LcdDriver.begin(numCols,numRows);
	LcdDriver.setCursor(0,0);
	timer = 0;
}

void loop()
{
	
	if(millis() - timer >= INTERVAL)
	{
		if((INTERVAL - 10 > 50) && curState	 == Playing	)
		{
			INTERVAL -= 10;
		}
		bState = ButtonTest();
		if(curState == Playing)
		{
			if((millis() - startTime)/1000 >= 30 && gameState == Game2)
			{
				curState = EndOfGame;
				delta = millis() - startTime;
			}
			int t = 12;
			char temp[t];
			Serial.readBytes(temp, t);
		
			if(bState == 3)
			{
				switchGame();
			}
			else if ( bState == 2)//short press
			{
				curState = NewGame;
			}
			LcdDriver.clear();
			shiftArrayRight(contents);
			insertRandomChars(contents);
			if(hasCollision(contents, curRow))
			{
				if(gameState == Game1)
				{
					if(--lives <= 0)
					{
						//newGame();
						curState = EndOfGame;
						delta = (millis() - startTime);
					}
					
				}
				else//is Game2
				{
					lives++;
					
				}
			}
			
			printArray(contents);

		}//end playing
		else if (curState == EndOfGame)
		{
			printEndData();
			INTERVAL	= INIT_INTERVAL	;
			if(bState==2 || bState == 3)
			{
				curState = Playing;
				newGame();
			}
		}
		
		else
		{
			newGame();
			curState = Playing;
		}
		//delta = (millis() - startTime);
		timer += INTERVAL;
	}
}
void printEndData()
{
			LcdDriver.clear();
			LcdDriver.setCursor(0,0);
			LcdDriver.print("End of game");
			LcdDriver.setCursor(0,1);
			LcdDriver.print(delta/1000);
			LcdDriver.print(" seconds");
			LcdDriver.setCursor(0,2);
			
			if(gameState == Game2)
			{
				LcdDriver.print(lives);
				LcdDriver.print(" collected");
			}
			LcdDriver.setCursor(0,3);
			LcdDriver.print("press button ");
			INTERVAL	 = INIT_INTERVAL	;
}
void prepopulateArray()
{
	for(int i =0; i<numRows-4; i++)
	{
		for(int j =0; j<numCols; j++)
		{
			contents[i][j] = emptyChar;
		}
	}
	for(int i =0; i<numRows;i++)
	{
		for(int j =0; j<numCols;j++)
		{
			contents[i][j] = (random(0,100) < 20)? spriteChar:emptyChar;
		}
		shiftArrayRight(contents);
	}
}
int switchGame()
{
	if( gameState == Game1)
	{
		gameState = Game2;
	}
	else if (gameState == Game2)
	{
		gameState = Game1;
	}
	newGame();
}
bool hasCollision(char inArray[4][20], int r)
{
	if(inArray[r][19] == spriteChar) return true;
	return false;
}
void newGame()
{
	LcdDriver.clear();
	//LcdDriver.print("You ran out of lives");
	prepopulateArray();

	startTime = millis();
	if(gameState == Game1)
	{
		lives = 5;
	}
	else if ( gameState == Game2)
	{
		lives =0;
	}
	curRow=0;
	INTERVAL = INIT_INTERVAL	;
}
void shiftArrayRight(char inArray[4][20])
{
	for(int i =0; i<4;i++)
	{
		for(int j =20-2; j>=0;j--)
		{
			inArray[i][j+1] = inArray[i][j];
		}
	}
}
void insertRandomChars(char inArray[4][20])
{
	for(int i =0; i<numRows; i++)//loop through rows
	{
		bool shouldPlaceNew = (random(0,100) < 15);
		if(shouldPlaceNew)
		{
			inArray[i][0] = spriteChar;
		}
		else
		{
			inArray[i][0] =emptyChar;
		}
	}
}
void MonitorA()
{
	if (digitalRead(2) == digitalRead(3)) //if inputA and input B are pins 2 and 3 respectively
	{
		incrementVHState(.25);
	}
	else
	{
		incrementVHState(-.25);
	}
	//bState = ButtonTest();
}
void MonitorB()
{
	if (digitalRead(2) == digitalRead(3)) //if inputA and input B are equal
	{
		incrementVHState(-.25);
	}
	else
	{
		incrementVHState(.25);
	}
	//bState = ButtonTest();
}
void incrementVHState(float n)
{
	
		curRow += n;
		if(curRow > numRows)
		{
			curRow = numRows;
		}
		if(curRow < 0)
		{
			curRow =0 ;
		}
		return;
}
void SerialPrintArray(char inArray[4][20])
{
	Serial.println();
	Serial.print(sizeof(contents[0]));
	Serial.println();
	for(int i =0; i<4;i++)//make back to 4
	{
		for (int j = 0; j < 20; j++)//make back to 20
		{
			Serial.print(inArray[i][j]);
			
		}
		Serial.println();
	}
}
void printArray(char inArray[4][20])
{
	for(int i =0; i<4;i++)
	{
		for (int j = 0; j < 20; j++)
		{
			LcdDriver.setCursor(j,i);
			LcdDriver.print(inArray[i][j]);
		}
	}
	LcdDriver.setCursor(19, curRow);
	LcdDriver.print('X');
	LcdDriver.setCursor(0,0);
	LcdDriver.print(lives);
}