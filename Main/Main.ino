#include <LiquidCrystal.h>
#include "ButtonDebounce.h"

volatile int encoderPosition = 0;
int numRows= 4;
int numCols= 20;

float curRow;
float curCol;
char contents[4][20];
int lives = 3;

unsigned long timer;
unsigned long startTime;
unsigned long delta;
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
char emptyChar ='_';
char spriteChar='*';

int INTERVAL = 700;
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
		bState = ButtonTest();
		if(curState == Playing)
		{
			if(millis()/1000 - startTime/1000 > 30 && Game2 == Game2)
			{
				curState = EndOfGame;

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
					}
					delta = millis() - startTime;
				}
				else//is Game2
				{
					lives++;
					
				}
			}
			if((INTERVAL - 50 > 70) && (gameState == Game2))
					{
						INTERVAL -= 50;
					}
			//SerialPrintArray(contents);
			printArray(contents);

		}//end playing
		else if (curState == EndOfGame)
		{
			LcdDriver.clear();
			LcdDriver.setCursor(0,0);
			LcdDriver.print("End of game");
			LcdDriver.setCursor(0,1);
			LcdDriver.print(delta/1000);
			LcdDriver.print(" seconds");
			LcdDriver.setCursor(0,3);
			LcdDriver.print("press button ");
			if(bState)
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
		delta = (millis() - startTime);
		timer += INTERVAL;
	}
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
			contents[i][j] = (random(0,100) < 10)? spriteChar:emptyChar;
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
		lives = 3;
	}
	else if ( gameState == Game2)
	{
		lives =0;
	}
	curRow=0;
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
/*void ButtonInitialize( int pin )
{
		ButtonPin = pin;
		pinMode( ButtonPin, INPUT );
}*/
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
// Function called in loop to check for button release.
// Returns a 1 on the buttons release.
/*int ButtonTest(  )
{
	// Read in the buttons current value.
	int Press = digitalRead( ButtonPin ); 
	int ReturnValue = 0;
	switch( buttonState )
	{
		case BS_Idle: // if we are waiting for a press,
			 if( Press == LOW )
			 {
				 // Once press occurs
				 bTime = millis();  // record time
				 buttonState = BS_Wait;        // and move to next state
			 }
			 break; 
		case BS_Wait: // button just went low
			 if( Press == HIGH ) // and now goes high
			 {
				 buttonState = BS_Idle;  // return to 0 state.
				 return 0;
			 }
			 
			 if( millis() - bTime >= 10 )
			 {
			 		buttonState = BS_Low;
					return 2; // 2- more than 10 millis
					 // move on to state two
			 }
				//buttonState = BS_LOW;
			 break;
		 case BS_Low:
			 if( Press == HIGH )
			 {
				 ReturnValue = 2; // Return 2 indicating release.
				 buttonState = BS_Idle;
				 if (millis() - bTime < 500)
				 {
					return 2;
				 }
				 else
				 {
					return 3;
				 }
			 } // End of high test.
			 break;
	} // End of switch on buttonState
	
	return ReturnValue;	
}*/
