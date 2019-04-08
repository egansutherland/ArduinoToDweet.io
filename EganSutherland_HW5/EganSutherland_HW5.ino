//
// Command processor program
// Written by Egan Sutherland
// 3/4/2019
// 

#include "DHT.h"
#include "Wire.h"
#include "Adafruit_Sensor.h"
#include "Adafruit_BMP280.h"
#include "I2Cdev.h"
#include "MPU6050.h"

#define RDA 0x80

char CR = 0x0D;
char BS = 0x08;
char ESC = 0x1B;
char crsArr[5];
const int crsTimerOriginal = 1600000000;
unsigned int crsTimer =  crsTimerOriginal;
unsigned int crsInd = 0;
int index = crsTimer;
int crs_on = 1;
const int blnkSpdOrigin = 500;
int blnkSpd = blnkSpdOrigin;
char blinkOption;
double myTone = 262;
DHT dht( 11, DHT22 );
Adafruit_BMP280 myBmp;
MPU6050 accro;

//Start tone generation variables
volatile unsigned char* myPin = 0; //change to hex value of pin connected to speaker

volatile unsigned char* port_b = (unsigned char*) 0x25; 
volatile unsigned char* ddr_b  = (unsigned char*) 0x24; 
volatile unsigned char* pin_b  = (unsigned char*) 0x23;

volatile unsigned char* myTCCR1A = (unsigned char*) 0x80; //x x x x x x WGm11 WGm10
volatile unsigned char* myTCCR1B = (unsigned char*) 0x81; //x x x WGm13 WGm12 CS12 CS11 CS10
volatile unsigned char* myTCCR1C = (unsigned char*) 0x82;
volatile unsigned int*  myTCNT1 = (unsigned int*) 0x84; //timer count register (16 bits)
volatile unsigned char* myTIMSK1 = (unsigned char*) 0x6F; //xxxxxxxTOIE1
volatile unsigned char* myTIFR1 = (unsigned char*) 0x36; // xxxxxxxTOV1

double freq = 440;
unsigned int ticks;

unsigned char myChar = 'l';
//Tone genearation variables end

//turns cursor on
void crson();

//turns cursor off
void crsoff();

//writes the main menu options to the screen
void writeMenu();

//maintains systems status
void syswrk();

//processes entered command
void cmdproc();

//updates rotating cursor
void crsrot();

//clears the serial monitor
void clrScrn();

//changes the speed of the rotating cursor based on input
void changeCrsSpeed();

//blinks the led with options to speed up, slow down, or reset the blink speed
void ledBlink();

//echoes strings from the keyboard
void kbdEcho();

//echoes single characters from the keyboard
void charEcho();

//helps charEcho accomplish its goals
int charEchoHelp();

void toneGen();

void myDelay( unsigned int myTicks );

//poll and display mpu data
void mpu();

//poll and display bmp data
void bmp();

//poll and display dht data
void dhtRead();

void setup() {
  // put your setup code here, to run once:
  Serial.begin( 9600 );
  crsArr[0] = 0x7C;
  crsArr[1] = 0x2F;
  crsArr[2] = 0x2D;
  crsArr[3] = 0x5C;
  *ddr_b |= 0x40;
  *ddr_b |= 0x80; //1000 0000
  *ddr_b &= 0b11011111;  //1101 1111
  *pin_b &= 0b11011111;  //1101 1111
  *myTCCR1A = 0;
  *myTCCR1B = 0;
  *myTCCR1C = 0;
  *myTIMSK1 = 0;

  Wire.begin();
  dht.begin();
  if( !myBmp.begin() )
  {
    Serial.print( "FAILED TO FIND BMP280" );
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  //crson();
  Serial.print( "MPU6050\r\n" );
  //Serial.print( "\n" );
  //Serial.print( CR );
  mpu();

  Serial.print( "BMP280\r\n" );
  //Serial.print( "\n" );
  //Serial.print( CR );
  bmp();
  
  Serial.print( "DHT22\r\n" );
  //Serial.print( "\n" );
  //Serial.print( CR );
  dhtRead();

  delay( 12000 );
}

void crson()
{
  crs_on = 1;
  bmp();
  //print menu
  writeMenu();
  
  //display rotating cursor and wait for input
  while( !Serial.available() )
  {
    syswrk();
    index--;
  }
  cmdproc();
}

void crsoff()
{
  crs_on = 0;
  Serial.print( BS );
  Serial.print( "\n" );
  Serial.print( CR );
  Serial.print( '>' );
}

void writeMenu()
{
  clrScrn();
  Serial.print( "Welcome to Egan's Arduino Mega Command Processor\n" );
  Serial.print( CR );
  Serial.print( "Please enter the desired function:\n" );
  Serial.print( CR );
  Serial.print( "1. Keyboard echo\n" );
  Serial.print( CR );
  Serial.print( "2. ASCII character echo\n" );
  Serial.print( CR );
  Serial.print( "3. Change the rotating cursor speed\n" );
  Serial.print( CR );
  Serial.print( "4. Blink LED\n" );
  Serial.print( CR );
  Serial.print( "5. Sound Tone\n" );
  Serial.print( CR );
  Serial.print( "6. Show MPU-6050 accelerometer and gyro data\n" );
  Serial.print( CR );
  Serial.print( "7. Show BMP280 barometric pressure data\n" );
  Serial.print( CR );
  Serial.print( "8. Show DHT22 temperature and humidity data\n" );
  Serial.print( CR );
  Serial.print( "\n" );
  Serial.print( CR );
}

void syswrk()
{
  if( !index )
    {
      crsrot();
      crsInd++;
      index = crsTimer;
    }
}

void cmdproc()
{
  char in;
  crsoff();
  in = Serial.read();
  if( in < 0x30 || in > 0x38 )
  {
    Serial.print( "Selection invalid. Please wait for reset and try again." );
    Serial.print( "\n" );
    Serial.print( CR );
    delay( 3000 );
    crson();
  }
  switch( in - '0' )
  {
    case 1:
    {
      //keyboard echo
      kbdEcho();
      break;
    }
    case 2:
    {
      charEcho();
      break;
    }
    case 3:
    {
      //change crs speed
      changeCrsSpeed();
      break;
    }
    case 4:
    {
      //blinky
      ledBlink();
      break;
    }
    case 5:
    {
      //sound
      toneGen();
      break;
    }

    case 6:
    {
      //MPU-6050
      mpu();
      break;
    }

    case 7:
    {
      //BMP280
      bmp();
      break;
    }

    case 8:
    {
      //DHT22
      dhtRead();
      break;
    }
  }
  crson();
}

void crsrot()
{
//  Serial.print( crsArr[ crsInd % 4 ] );
//  Serial.print( BS );
}

void clrScrn()
{
  Serial.write( ESC );
  Serial.print( "[2J" );
  Serial.write( ESC );
  Serial.print( "[H" );
}

void changeCrsSpeed()
{
  char wait = 0;
  clrScrn();
  Serial.print( "Cursor Speed Changer\n" );
  Serial.print( CR );
  Serial.print( "\n" );
  Serial.print( CR );
  Serial.print( "Please enter - to decrease speed, + to increase speed, or r to reset speed.\n" );
  Serial.print( CR );
  Serial.print( "  > " );
  while( !Serial.available() )
  {
    //wait for input
  }
  wait = Serial.read();
  Serial.print( wait );
  if( wait == '-' )
  {
    crsTimer = crsTimer / 3;
    Serial.print( "\n" );
    Serial.print( "Cursor speed successfully decreased. Returning to Main Menu in 3 seconds." );
    delay( 3000 );    
  }
  else if( wait == '+' || wait == '=' )
  {
    crsTimer = crsTimer * 3;
    Serial.print( "\n" );
    Serial.print( "Cursor speed successfully increased. Returning to Main Menu in 3 seconds." );
    delay( 3000 );
  }
  else if( wait == 'r' )
  {
    crsTimer = crsTimerOriginal;
    Serial.print( "\n" );
    Serial.print( "Cursor speed successfully reset. Returning to Main Menu in 3 seconds." );
    delay( 3000 );
  }
  else
  {
    Serial.print( "\n" );
    Serial.print( CR );
    Serial.print( "Invalid Entry. Returning to Main Menu in 3 seconds. Cursor Speed not updated." );
    delay( 3000 );
  }
}

void ledBlink()
{
  //prep blinking
  pinMode( LED_BUILTIN, OUTPUT );
  digitalWrite( LED_BUILTIN, HIGH );   // turn the LED on
  int counter = 0;

  //maintain system clarity
  clrScrn();
  Serial.print( "LED now blinking. Press + to increase blink speed.\n" );
  Serial.print( CR );
  Serial.print( "Press - to decrease blink speed. Press r to reset blink speed.\n" );
  Serial.print( CR );
  Serial.print( "Press q to exit blink and return to main menu.\n" );
  Serial.print( CR );

  //blink loop
  while( !Serial.available() )
  {
      digitalWrite( LED_BUILTIN, HIGH );   // turn the LED on
      delay( blnkSpd );
      digitalWrite( LED_BUILTIN, LOW );    // turn the LED off 
      delay( blnkSpd );
  }

  blinkOption = Serial.read();

  if( ( blinkOption == '+' || blinkOption == '=' ) && blnkSpd > 0 )
  {
    blnkSpd -= 100;
  }
  else if( blinkOption == '-' && blnkSpd < 1001 )
  {
    blnkSpd += 100;
  }
  else if( blinkOption == 'r' )
  {
    blnkSpd = blnkSpdOrigin;
  }
  else if( blinkOption == 'q' )
  {
    digitalWrite( LED_BUILTIN, LOW );
    return;
  }

  ledBlink();

  digitalWrite( LED_BUILTIN, LOW );
}

void kbdEcho()
{
  int i = 0;
  char echo[ 100 ];

  clrScrn();
  Serial.print( "This function echoes any entered string under 100 characters long.\n" );
  Serial.print( CR );
  Serial.print( "Enter desired string then press enter to echo it. The function will\n" );
  Serial.print( CR );
  Serial.print( "then echo the entered string and wait for additional input.\n" );
  Serial.print( CR );
  Serial.print( "ESC followed by enter will exit to main menu.\n" );
  Serial.print( CR );
  Serial.print( "\n" );
  Serial.print( CR );
  Serial.print( "> " );

  while( echo[ i - 1 ] != CR && i < 100 )
  {
    while( !Serial.available() ){}
    echo[ i ] = Serial.read();
    Serial.print( echo[ i ] );
    i++;
  }

  if( echo[ 0 ] == 0x1B )
  {
    return;
  }

  Serial.print( "\n" );
  Serial.print( CR );
  
  for( int j = 0; j < i; j++ )
  {
    Serial.print( echo[ j ] );
  }

  Serial.print( "\n" );
  Serial.print( CR );
  Serial.print( "\n" );
  Serial.print( CR );
  Serial.print( "Press any key to eneter another string to echo.\n" );
  Serial.print( CR );
  while( !Serial.available() ){};
  Serial.read();
  
  kbdEcho();
}

void charEcho()
{
  int exitFlag = 0;
  clrScrn();

  Serial.print( "This function echoes any single character from the keyboard.\n" );
  Serial.print( CR );
  Serial.print( "Entering ESC will to main menu.\n" );
  Serial.print( CR );
  Serial.print( "\n" );
  Serial.print( CR );

  while( exitFlag == 0 )
  {
    Serial.print( "\n" );
    Serial.print( CR );
    Serial.print( "> " );
    exitFlag = charEchoHelp();
  }
  delay( 3000 );
}

int charEchoHelp()
{
  while( !Serial.available() ){}
  char wow = Serial.read();
  Serial.print( wow );
  Serial.print( "\n" );
  Serial.print( CR );
  Serial.print( wow );
  Serial.print( ": " );// this is what you added in os
  Serial.print( (int) wow );// also this
  if( wow == 0x1B )
  {
    return 1;
  }
  return 0;
}

void toneGen()
{
  clrScrn();
  Serial.print( "Welcome to the Tone Generation Function!\n" );
  Serial.print( CR );
  Serial.print( "The tone begins at 440 Hz.\n" );
  Serial.print( CR );
  Serial.print( "Pressing + raises the tone by 20 Hz.\n" );
  Serial.print( CR );
  Serial.print( "Pressing - lowers the tone by 20 Hz.\n" );
  Serial.print( CR );
  Serial.print( "The floor is 130 Hz.\n" );
  Serial.print( CR );
  Serial.print( "Pressing r resets the tone.\n" );
  Serial.print( CR );
  Serial.print( "Pressing ESC turns off the tone and returns to the main menu.\n" );
  Serial.print( CR );

  //clrScrn();

  int escFlag = 1;
  double periodWave = 0;
  double periodClk = 0;
  double myTicks = 0;
  
  while( escFlag == 1 )
  {
    //caclulate ticks
    periodWave = 1/(2*freq);
    periodClk = 0.0000000625;
    myTicks = periodWave/periodClk;

    *port_b |= 0x40;
    myDelay( (unsigned int) myTicks );
    *port_b &= 0xBF;
    myDelay( (unsigned int) myTicks );
    if( Serial.available() )
    {
      myChar = Serial.read();
      if( myChar == ESC )
      {
        escFlag = 0;
      }
      if( myChar == '+' || myChar == '=' )
      {
        freq += 20;
      }
      if( myChar == '-' )
      {
        freq -= 20;
      }
      if( myChar == 'r' )
      {
        freq = 440;
      }
      if( freq < 130 )
      {
        freq = 130;
      }
    }
  }
}


void myDelay( unsigned int myTicks )
{
  *myTCCR1B &= 0xF8;
  *myTCNT1 = ( unsigned int ) ( 65536 - myTicks );
  *myTCCR1B |= 0x01;
  while( ( *myTIFR1 & 0x01 ) == 0 ){}
  *myTCCR1B &= 0xF8;
  *myTIFR1 |= 0x01;
}

//poll and display mpu data
void mpu()
{
  //unsigned int counter = 0;
  //bool exitFlag = true;
  //unsigned char dummy = ' ';
  int accx, accy, accz;
  int gyrox, gyroy, gyroz;

  accx = accy = accz = 0;
  gyrox = gyroy = gyroz = 0;
  
  //clrScrn();
  /*Serial.print( "Welcome to the MPU6050 Accelerometer and Gyro\n" );
  Serial.print( CR );
  Serial.print( "Press ESC to return to main menu\n" );
  Serial.print( CR );
  Serial.print( "\n" );
  Serial.print( CR );*/

  accro.getMotion6( &accx, &accy, &accz, &gyrox, &gyroy, &gyroz );

 // if( accx || accy || accz || gyrox || gyroy || gyroz )
 // {
    /*Serial.print( "Accelerometer: x: " );
    Serial.print( accx );
    Serial.print( "\ty: " );
    Serial.print( accy );
    Serial.print( "\tz: " );
    Serial.print( accz );
    Serial.print( "\n" );
    Serial.print( CR );
    Serial.print( "Gyro: x: " );
    Serial.print( gyrox );
    Serial.print( "\ty: " );
    Serial.print( gyroy );
    Serial.print( "\tz: " );
    Serial.print( gyroz );
    Serial.print( "\n" );
    Serial.print( CR );*/
        Serial.print( accx );
        Serial.print( "\t" );
        Serial.print( accy );
        Serial.print( "\t" );
        Serial.print( accz );
        Serial.print( "\r\n" );
        //Serial.print( CR );
        Serial.print( gyrox );
        Serial.print( "\t" );
        Serial.print( gyroy );
        Serial.print( "\t" );
        Serial.print( gyroz );
        Serial.print( "\r\n" );
        //Serial.print( CR );    
 // }

/*  while( exitFlag )
  {
    if( Serial.available() )
    {
      dummy = Serial.read();
      if( dummy == ESC )
      {
        exitFlag = false;
      }
    }

    delay( 1 );
    counter++;

    if( counter >= 3000 )
    {
      counter = 0;
      
      accro.getMotion6( &accx, &accy, &accz, &gyrox, &gyroy, &gyroz );

     // if( accx || accy || accz || gyrox || gyroy || gyroz )
   //   {
        //Serial.print( "Accelerometer: x: " );
        Serial.print( accx );
        Serial.print( "\t" );
        Serial.print( accy );
        Serial.print( "\t" );
        Serial.print( accz );
        Serial.print( "\t" );
        //Serial.print( CR );
        //Serial.print( "Gyro: x: " );
        Serial.print( gyrox );
        Serial.print( "\t" );
        Serial.print( gyroy );
        Serial.print( "\t" );
        Serial.print( gyroz );
        Serial.print( "\t" );
        //Serial.print( CR );    
   //   }
    }
    
  }*/
}

//poll and display bmp data
void bmp()
{
  float temp = 0;
  float pressure = 0;
  float alt = 0;
  //bool exitFlag = true;
  //unsigned int counter = 0;
  //unsigned char dummy = ' ';
  
  //clrScrn();

  /*Serial.print( "Welcome to the BMP280 Temperature, Pressure, and Altitude monitor\n" );
  Serial.print( CR );
  Serial.print( "\n" );
  Serial.print( CR );*/

  temp = myBmp.readTemperature();
  pressure = myBmp.readPressure();
  alt = myBmp.readAltitude( 1013.25 );

//  if( temp != 0 && pressure != 0 && alt != 0 )
//  {
    //Serial.print( "Temperature: " );
    Serial.print( temp );
    Serial.print( "\r\n" );
    //Serial.print( CR );
    //Serial.print( "Pressure: " );
    Serial.print( pressure );
    Serial.print( "\r\n" );
    //Serial.print( CR );
    //Serial.print( "Altitude: " );
    Serial.print( alt );
    //Serial.print( CR );
    Serial.print( "\r\n" );
    Serial.print( CR );
}

/*  while( exitFlag )
  {
    if( Serial.available() )
    {
      dummy = Serial.read();
      if( dummy == ESC )
      {
        exitFlag = false;  
      }
    }

    delay( 1 );
    counter++;

    if( counter >= 5000 )
    {
      counter = 0;

      temp = myBmp.readTemperature();
      pressure = myBmp.readPressure();
      alt = myBmp.readAltitude( 1013.25 );
      
      if( temp != 0 && pressure != 0 && alt != 0 )
      {
    //Serial.print( "Temperature: " );
    Serial.print( temp );
    Serial.print( "\t" );
    //Serial.print( CR );
    //Serial.print( "Pressure: " );
    Serial.print( pressure );
    Serial.print( " Pa" );
    //Serial.print( CR );
    //Serial.print( "Altitude: " );
    Serial.print( alt );
    Serial.print( "\t" );
    //Serial.print( CR );
    //Serial.print( "\n" );
    //Serial.print( CR );
      }
    }
  }
}*/

//poll and display dht data every 2 minutes, starting at call of this function
void dhtRead()
{
  //unsigned int counter = 0;
  //unsigned char dummy = ' ';
  //bool exitFlag = true;
  float humidity = 0;
  float temperature = 0;

  //clrScrn();

  /*Serial.print( "Welcome to the DHT22 Temperature and humidity monitor\n" );
  Serial.print( CR );
  Serial.print( "Press ESC to exit to main menu.\n" );
  Serial.print( CR );
  Serial.print( "\n" );
  Serial.print( CR );*/

  humidity = dht.readHumidity();
  temperature = dht.readTemperature();

  //if( humidity != 0 && temperature != 0 )
  //{
    //Serial.print( "Current Humidity: " );
    /*Serial.print( "\r\n\r\n\r\n" );
    Serial.print( humidity );
    Serial.print( "\r\n" );
    Serial.print(":LSKJDHFK:LDSJHJF");
    Serial.print( temperature );
    Serial.print( "\r\n" );*/
  //}
  
/*  while( exitFlag )
  {
    if( Serial.available() )
    {
      dummy = Serial.read();
      if( dummy == ESC )
      {
        exitFlag = false;
      }
    }
      
    delay( 1 );
    counter++;

    if( counter >= 120000 )
    {
      counter = 0;

      humidity = dht.readHumidity();
      temperature = dht.readTemperature();

      if( humidity != 0 && temperature != 0 )
      {
        //Serial.print( "Current Humidity: " );
        Serial.print( humidity );
        Serial.print( "\t" );
        //Serial.print( CR );
        //Serial.print( "Current Temperature: " );
        Serial.print( temperature );
        Serial.print( "\t" );
        //Serial.print( CR );
        //Serial.print( "\n" );
        //Serial.print( CR );
      }
      else
      {
        //Serial.print( "Failure to read temperature or humidity, retrying...\n" );
        //Serial.print( CR );
        counter = 119999;
      }

      humidity = 0;
      temperature = 0;
    }
  }*/
}
