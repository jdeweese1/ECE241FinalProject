#include <LiquidCrystal.h>
#include <ButtonDebounce.h>

volatile int encoderPosition = 0;
int row;
int col;
int numRows= 4;
int numCols= 20;

float curRow;
float curCol;
char contents[4][20];
int lives = 3;

unsigned long timer;
LiquidCrystal LcdDriver(11,9,5,6,7,8);

// Set up pin and button state.
int bState;
int ButtonPin = 4;
enum buttonStates {BS_Idle,BS_Wait,BS_Low};
buttonStates buttonState = BS_Idle;
enum GameStates { Pause,NewGame, Playing };
GameStates curState = Playing;
unsigned long buttonTimer;
char emptyChar ='_';
char spriteChar='*';

void setup()
{
	//pinMode(12,OUTPUT);
	pinMode(4,INPUT);
	for(int i =0; i<numRows; i++)
	{
		for(int j =0; j<numCols; j++)
		{
			contents[i][j] = emptyChar;
		}
	}
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
	if(millis() -timer >700)
	{
		int t = Serial.available();
		if(t)
		{
			char temp[t];
			Serial.readBytes(temp, t);
			bState = ButtonTest();
		
			if(bState == 3)
			{
				curState = NewGame;
				newGame();
			}
			if ( bState == 2)
			{
				curState = Pause;
			}
			

			LcdDriver.clear();
			shiftArrayRight(contents);
			insertRandomChars(contents);
			if(hasCollision(contents, curRow))
			{
				if(--lives <= 0)
				{
					newGame();
					curState = NewGame;
				}
			}
			SerialPrintArray(contents);
		}
		
		printArray(contents);
		

		timer += 700;
	}
}
bool hasCollision(char inArray[4][20], int r)
{
	if(inArray[r][20] == spriteChar) return true;
	return false;
}
void newGame()
{
	LcdDriver.clear();
	LcdDriver.print("You ran out of lives");
	for(int i=0; i<numRows;i++)
	{
		for(int j =0; j<numCols;j++)
		{
			contents[i][j]= emptyChar;		
		}
	}
	lives = 3;
	curRow=0;
}
void shiftArrayRight(char inArray[4][20])
{
	for(int i =0; i<4;i++)
	{
		for(int j =20-1; j>=0;j--)
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
	inArray[3][0] = spriteChar;
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
	
	bState = ButtonTest();
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
	bState = ButtonTest();
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
void ButtonInitialize( int pin )
{
		ButtonPin = pin;
		pinMode( ButtonPin, INPUT );
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
// Function called in loop to check for button release.
// Returns a 1 on the buttons release.
int ButtonTest(  )
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
				 buttonTimer = millis();  // record time
				 buttonState = BS_Wait;        // and move to next state
			 }
			 break; 
		case BS_Wait: // button just went low
			 if( Press == HIGH ) // and now goes high
			 {
				 buttonState = BS_Idle;  // return to 0 state.
			 }
			 
				 if( millis() - buttonTimer >= 10 )
				 {
						ReturnValue = 2; // Return 1 indicating pressed.
						buttonState = BS_Low; // move on to state two
				 }
					//buttonState = BS_LOW;
			 
			 break;
		 case BS_Low:
			 if( Press == HIGH )
			 {
				 ReturnValue = 2; // Return 2 indicating release.
				 buttonState = BS_Idle;
				 if (millis() - buttonTimer < 500)
				 {
					return 2;
				 }
				 else if(millis() - buttonTimer > 500)
				 {
					return 3;
				 }
			 } // End of high test.
			 break;
	} // End of switch on buttonState
	
	return ReturnValue;	
}
