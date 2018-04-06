#include <LiquidCrystal.h>
//#include <SW_SPI.h>
#include <SPI.h>

volatile int encoderPosition = 0;
int row;
int col;
int numRows= 8;
int numCols= 8;
int temp =0;

float curRow;
float curCol;

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

/*#define MAX7219_TEST 0x0f00
#define MAX7219_BRIGHTNESS 0x0a00
#define MAX7219_SCAN_LIMIT 0x0b00
#define MAX7219_DECODE_MODE 0x9000
#define MAX7219_SHUTDOWN 0x0c00
SPISettings settings(8000000, MSBFIRST,SPI_MODE0);
*/

void setup()
{
  //pinMode(12,OUTPUT);
  pinMode(4,INPUT);\
  /*
  digitalWrite(10,LOW);
SW_SPI_Initialize();
digitalWrite(10,HIGH);
pinMode(10, OUTPUT);
  SPI.begin();
  //Configure SPI hardware

  digitalWrite(10,LOW);
  // Set up display These need to be sent in this fashion in setup.
  SPI.transfer16(MAX7219_TEST + 0x01); // Turn on all the LEDs.
    digitalWrite(10,HIGH);

  delay(100); // One time we can use a delay.
  digitalWrite(10,LOW);
  SPI.transfer16(MAX7219_TEST + 0x00); // all LEDS off.
  digitalWrite(10,HIGH);

  digitalWrite(10,LOW);
  SPI.transfer16(MAX7219_DECODE_MODE + 0x00); // Disable BCD mode.
  digitalWrite(10,HIGH);

  digitalWrite(10,LOW);
  SPI.transfer16(MAX7219_BRIGHTNESS + 0x03); // Use lower intensity.
  digitalWrite(10,HIGH);

  digitalWrite(10,LOW);
  SPI.transfer16(MAX7219_SCAN_LIMIT + 0x0f); // Scan all  playdigits.
  digitalWrite(10,HIGH);

  digitalWrite(10,LOW);
  SPI.transfer16(MAX7219_SHUTDOWN + 0x01); // Turn on chip.
  digitalWrite(10,HIGH);
  */
  
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
  
  if(millis() -timer >100)
  {
    bState = ButtonTest();
    
    if(bState == 3)
    {
      /*curCol = 0;
      curRow =0;*/
      curState = Horizontal;
    }
    if ( bState == 2)
    {
      curState = Vertical;
    }
    
    timer += 100;
    LcdDriver.clear();
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
      
      /*digitalWrite(10,LOW);
      SPI.beginTransaction(settings);
      SPI.transfer16(0xff);
      
      digitalWrite(10,HIGH);
      SPI.endTransaction();*/
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


void VHInitialize()
{
  curState = Vertical;
}
void toggleVH()
{
  if(curState == Vertical)
  {
    curState = Horizontal;
    return;
  }
  curState = Vertical;
}
void incrementVHState(float n)
{
  if(curState == Vertical )
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
  curCol +=n;
  if(curCol > numCols)
  {
    curCol = numCols;

  }
  else if (curCol< 0)
  {
    curCol =0;
  }

}

void ButtonInitialize( int pin )
{
    ButtonPin = pin;
    pinMode( ButtonPin, INPUT );
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
