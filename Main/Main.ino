#include <LiquidCrystal.h>
//#include <SW_SPI.h>
//#include <SPI.h>
#include <ButtonDebounce.h>

volatile int encoderPosition = 0;
int row;
int col;
int numRows= 4;
int numCols= 16;
int temp =0;
// char contents[numRows][numCols];

float curRow;
float curCol;
char contents[4][16] ;

unsigned long timer;
LiquidCrystal LcdDriver(11,9,5,6,7,8);


// Set up pin and button state.
int bState;
int ButtonPin = 4;
enum buttonStates {BS_Idle,BS_Wait,BS_Low};
buttonStates buttonState = BS_Idle;
enum controlStates { Horizontal,Vertical};
controlStates curState = Vertical;
unsigned long buttonTimer;


void setup()
{
	
	//pinMode(12,OUTPUT);
	pinMode(4,INPUT);
	for(int i =0; i<numRows; i++)
	{
		for(int j =0; j<numCols; j++)
		{
			contents[i][j] = ' ';
		}
	}
	
	attachInterrupt(digitalPinToInterrupt(2), MonitorA, CHANGE);
	attachInterrupt(digitalPinToInterrupt(3), MonitorB, CHANGE);
	ButtonInitialize(4);
	LcdDriver.begin(numCols,numRows);
	LcdDriver.setCursor(0,0);
	LcdDriver.print("A");
	timer = 0;
}

void loop()
{
	
	if(millis() -timer >200)
	{
		bState = ButtonTest();
		
		if(bState == 3)
		{
			newGame();
		}
		if ( bState == 2)
		{
			curState = Vertical;
		}
		
		timer += 100;
		LcdDriver.clear();
		/*
			LcdDriver.setCursor(0,0);
			if(bState != 0) temp = bState;
			LcdDriver.print(temp);
			LcdDriver.print(" ");
			LcdDriver.print((curState==Vertical)? "V":"H");
			LcdDriver.setCursor(0,1);
			LcdDriver.print("r");
			LcdDriver.print((int)curRow);
			LcdDriver.print("c");
			LcdDriver.print((int)curCol);
			*/
			
			/*digitalWrite(10,LOW);
			SPI.beginTransaction(settings);
			SPI.transfer16(0xff);
			
			digitalWrite(10,HIGH);
			SPI.endTransaction();*/
	}
}
void newGame()
{
	for(int i=0; i<numRows;i++)
	{
		for(int j =0; j<numCols;j++)
		{
			contents[i][j]=' ';		}
	}
}
void shiftArrayRight(char inArray[4][16])
{

}
void insertRandomChars(char inArray[4][16])
{
	for(int i =0; i<numRows;i++)
	{
		bool shouldPlaceNew = (random(0,100) < 20);
		if(shouldPlaceNew)
		{
			inArray[i][0] = '*';
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
void printArray(char inArray[4][16])
{
	for(int i =0; i<4;i++)
	{
		for (int j = 0; j < 16; i++)
		{
			LcdDriver.setCursor(i,j);
			LcdDriver.print(inArray[i][j]);
		}
	}
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
