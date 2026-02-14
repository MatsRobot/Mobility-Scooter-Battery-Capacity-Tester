/*
------------------------------------------------------------------------------------------- //
**************************     Lead Acid Battery Capacity Tester   Version 1.2  *********************** //
------------------------------------------------------------------------------------------- //

In Mobility scooters there are two 12v batteris connected in series to create the required 24v operating voltage.
hence there are three accessible points, 0V, 12V and 24V. To get the voltage of the second battery we need to measure 
the total voltage of the first+second battery and subtract it from the voltage of the first battery.

The Lead acid battery will get damages if discharged over 30%, so the there is a significant focus of ensuring that the
voltage .
When a lead battery sits below 50% state of charge, the rate of growth & accumulation of lead sulphate 
crystals increases substantially and blocks access to the plates for the electrolyte which diminishes battery capacity. 
The battery gets damaged that cannot be reversed fully. 
This means that the only way to measure the capacity of the battery and the distance that the mobility scooter could travel
on a full charge is by discharging it to 50% in a control environment and measure the voltages vs time taken for the discharge.

The circuit was designed to be used with either one (12v) or two lead acid batteries in series (24v).

With Lead acid batteris we talk 12v batteries but the actual voltage values vs capacity are:

  12.89v = 100% // Battery full and standing for a while
  12.65 = 80%   // Safe discharge levwel
  12.23 = 50%   // below this level battery start to get damaged
  11.96 = 30%   // below this value the battery will not recover fully
  11.63 = 0%    // Totally Dead from capacity point of view

This project uses module 'RFID_Clocking_Machine_V4.4' for the data logging and forms the base of the unit.
The additional follwoing pins are assigned to the Battery Tester and a small circuit was designed to take the voltage from
the batteries to be measured.

A2 -  B1 (The first Battery voltage stepped down 0-5 represents 0-15)
A3 -  B2 (The first + second battery voltage stepped down 0-5 represents 0-25)

D5 -  This is the Auto power off and needs to be high when the device switched on and go down
      to turn the power off if the voltage of the battery gets below the damage threshold (12v)
Discharging standard lead-acid batteries to a low level can damage the plates due to shedding of lead sulfate 
from the plates. Thus, for best life, it it recommended that standard Pb-acid batteries be discharged to no more 
than 50% of it's capacity, which is about 12V for a nominal 12.6V battery.

A6 - Voltage Threshold select pot

D3 - Discharge Relay 1 for the first Battery
D4 - Discharge Relay 2 for the second Battery (If connected)


*/


//
// ------------------------------------------------------------------------------------------- //
//                                         Include files                                       //
// ------------------------------------------------------------------------------------------- //
#include <SD.h>                // include the SD library code for the SD card Module
#include <Wire.h>              // include the I2C library for RTC device and any other
#include <RTClib.h>            // include the RTC library code this also includes RTClib.cpp

#define I2C_ADDRESS 0x3C      // OLED Display 0X3C+SA0 - 0x3C or 0x3D
#include "SSD1306Ascii.h"
#include "SSD1306AsciiAvrI2c.h"

// ------------------------------------------------------------------------------------------- //
//                             Variables and Pin Definitions                                   //
// ------------------------------------------------------------------------------------------- //


#define BuzzerPin 9      // Buzzer Connected to Digital Pin 9

// creates an RTC object in the code variables for reading the RTC time & handling the INT(0) interrupt it generates
RTC_DS3231 RTC; 
#define DS3231_I2C_ADDRESS 0x68
int temp3231; //variables for reading the DS3231 RTC temperature register
byte tMSB = 0;
byte tLSB = 0;

// SD card Reader, use digital pin 10 for the SD cs line
const int chipSelect = 10;
File logfile;             // the SD logging file

SSD1306AsciiAvrI2c oled;  // Initialise OLED display
char Line1[25];           //Display line 1
char Line2[25];           //Display line 2
char Line3[25];           //Display line 3
char InfoLine[20];        //Display line 3
char ErrorMessage[25];    // String to hold error messages  

/*
Char arrays are used instead of string variables because the undefined variables were causing the the stack overflow
and crashed the programme as it was too long to fit in the memory, so instead all the variables are defined as
finite size char arrays.  
The complexity is to read and write one character at the time into these arrays and the last Character should be Null.
*/

char b[5];              // 4 digits + \0 to hold the temporary date/time byte
char TimeDate_Tag[20];  // array to hold the DateTime
char Time_Tag[10];      // array to hold the Time 
char Date_Tag[10];      // array to hold the Date

int BlueLED = 14;       // Place an LED to use A0 on Pin 14

// Added test board specific variables
int PowerLatchPin = 5;  // When high it latches the power on and when Low it switches the power off
int B1Pin = A2;         // Battery 1 V 0 to 15 ---> 0 to 5 connected to analog pin 2
double V1 =0.0;         // Holds Battery 1 voltage
double V1Cal = 1.00;    // Holds software caliboration value although caliboration is done with the pots
char V1Char[8];         // 7 digits + \0 to hold the Battery1 value in string format

int B2Pin = A3;         // Battery 1 + Battery 2 V 0 to 25 ---> 0 to 5 connected to analog pin 3
double V2 =0.0;         // Holds Battery 1+2 voltage 
double V2Cal = 1.00;    // Holds software caliboration value although caliboration is done with the pots
char V2Char[8];         // 7 digits + \0 to hold the BatteryTotal value in string format

double B2 =0.0;         // Holds Battery 2 voltage (Total Batteries voltage - Battery 1) 
char B2Char[8];         // 7 digits + \0 to hold the Battery2 value in string format

// Loop variables
int i;
int LoopDisplay = 50;   // Start with the last loop so it logs the data on switch on
int LoopLog = 50;       // Number of loops to take a log (it takes 2s to complete each loop)

double SafeThreshold = 12.23; // Safe battery discharge threshold to 50% capacity set by the pot
char SafeThresholdChar[8];    // 7 digits + \0 to hold the Safe Discharge Threshold value in string format
int ThresholdPin = A6;

int B1Latch = 1;              // disengage the load for Battery 1 once the threshold is reached 
int B2Latch = 1;              // disengage the load for Battery 2 once the threshold is reached 
int B1DischargePin = 7;       // Battery 1 Discharge relay pin D7
int B2DischargePin = 8;       // Battery 1 Discharge relay pin D8

// ------------------------------------------------------------------------------------------- //
//                                       Error Trapping                                        //
// ------------------------------------------------------------------------------------------- //

void error(char *str)
{
  Serial.print("error: ");
  Serial.println(str);

// Print Error message to the LCD.
   sprintf(Line1, "error: ");   
   sprintf(Line2, str);
   sprintf(Line3, " ");  
   DisplayText(Line1, Line2, Line3);

 // On Error flash the LED and set Buzzer for 10 Beeps 
  for (int i = 0; i<5; i++) // Number of Flash and 1/2 number of Beeps
 {
 //    digitalWrite(BlueLED, HIGH);    // turn the Blue LED on

/*
// Using Buzzer
  digitalWrite(BuzzerPin, HIGH);
  delay(200); 
  digitalWrite(BuzzerPin, LOW);
  delay(200);
*/

}
  while(1);                       // wait while there is an error
} // end of void error(char *str)



// ------------------------------------------------------------------------------------------- //
//                                       Display Procedure                                        //
// ------------------------------------------------------------------------------------------- //

void DisplayText(char *Line1, char *Line2, char *Line3)
{
  oled.clear();
  
  oled.setFont(Arial_bold_14);  // Set font System5x7
  oled.setCursor(0, 0);  
  oled.println(F("Battery Test V1.2"));


// 3 line text
  oled.setFont(Arial_bold_14);  // Set font
  oled.setCursor(0, 2);  
  oled.println(Line1);
  oled.setCursor(0, 4);  
  oled.print(Line2);
  oled.setCursor(0, 6);  
  oled.print(Line3);
} // end of void DisplayText



// ------------------------------------------------------------------------------------------- //
//                                         Set up Procedures                                   //
// ------------------------------------------------------------------------------------------- //

void setup(void)
{

  oled.begin(&Adafruit128x64, I2C_ADDRESS);

// Setting the SPI pins high helps some sd cards go into sleep mode 
   pinMode(chipSelect, OUTPUT); 
   digitalWrite(chipSelect, HIGH); //Always pullup the CS pin with the SD library

   delay(100);

// Set up Serial Monitor Bit rate and Print Serial Ready
  Serial.begin(9600);
  delay(100);                        
  Serial.flush();
  Serial.println("Serial Ready");  // Write with the carrage return and line feed

  pinMode(13, OUTPUT);     //the LED on board to Output even if not used.
// Set the LED and Buzzer pins to output
  pinMode(BlueLED, OUTPUT);
  pinMode(BuzzerPin, OUTPUT);

  pinMode(PowerLatchPin, OUTPUT);
  digitalWrite(PowerLatchPin, HIGH);    // turn the Power latch on

  pinMode(B1DischargePin, OUTPUT);
  pinMode(B2DischargePin, OUTPUT);

  
// *** Initialize the SD card 
// **************************
  Serial.print("Initializing SD card...");   // Write to Serial Monitor

// make sure that the default chip select pin is set to output even if NOT Used
  pinMode(10, OUTPUT); // SD Chip Select pin
  
// see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) 
  {
   strcpy(ErrorMessage,"Card Failed");
   error(ErrorMessage);
  }
  
  Serial.println("card initialized.");

// Write to the same file (8 Char file name)
   char filename[] = "DataLog.CSV";
       logfile = SD.open(filename, FILE_WRITE); 
   if (! logfile) 
   {
    strcpy(ErrorMessage,"Could not create file");
    error(ErrorMessage);
   }
   
 // Display the file name on Serial Monitor
  Serial.print("Logging to: ");
  Serial.println(filename);

// *** Initialize and connect to RTC DS1307
// ****************************************
// Begin the I2C interface connection
  Wire.begin(); 
  if (!RTC.begin()) 
  {
    logfile.println("RTC fail");
    strcpy(ErrorMessage,"Clock (RTC) Failed");
    error(ErrorMessage);
      Serial.println("RTC fail");
  }
   
// ****** Set Clock time
// *********************
//RTC.adjust(DateTime(__DATE__, __TIME__)); //******* uncomment this line to set time to system time 

/* to avoid time getting reset all the time remove this code from the final code
   RTC.adjust(DateTime("DEC 31 2011","12:59:45")); // Setting the time to a fixed value. 
   RTC.adjust(DateTime(12,8,25,13,00,00));     // DateTime (year, month, day, hour, min, sec);
*/


} // end of void setup(void)


// ------------------------------------------------------------------------------------------- //
//                                  Repeated (LOOP) Procedures                                 //
// ------------------------------------------------------------------------------------------- //

void loop(void)
{
  V1 = analogRead(B1Pin);  // read the input pin
  V2 = analogRead(B2Pin);  // read the input pin

  Serial.print("V1:");   Serial.print(V1);   Serial.print(" V2:");  Serial.println(V2);

  V1 = (double)(V1*16.05/1024) * V1Cal;  // 16.00 is equivilant to 5V (13.08==4.09) 
  V2 = (double)(V2*28.08/1024) * V2Cal;   // 28.01 is equivalant to 5v (26.22==4.68)
  B2 = (V2 - V1);

  SafeThreshold = analogRead(ThresholdPin);           //Read the pot value to set safe discharge threshold
  SafeThreshold = 11.00+ (SafeThreshold *2 /1024);    // 2v range,  Min 11V to 13V

// Create String variable from floating numbers
  dtostrf(V1, 4, 2, V1Char);  //mininum width 4, precision 2 ; float value is copied onto buff
  dtostrf(V2, 4, 2, V2Char);  //mininum width 4, precision 2 ; float value is copied onto buff
  dtostrf(B2, 4, 2, B2Char);  //mininum width 4, precision 2 ; float value is copied onto buff
  dtostrf(SafeThreshold, 4, 2, SafeThresholdChar);  //mininum width 4, precision 2 ; float value is copied onto buff

// Reset the arrow once it completed a line using Modulus operator
if (LoopDisplay/5 == LoopDisplay%5  || LoopDisplay == 50)
    {  
      strcpy(Line1,"Safe: ");
      strcat(Line1,SafeThresholdChar);
      strcat(Line1," ");
    }
  strcat(Line1,">");
  sprintf(Line2, "V1: %s  V2: %s", V1Char, V2Char); 
  sprintf(Line3, "B1: %s  B2: %s", V1Char, B2Char);
  DisplayText(Line1, Line2, Line3);

  digitalWrite(BlueLED, HIGH);       // turn the Blue LED on 
  delay(100); // Delay so it doesn't log twice if card is held for a long time
  digitalWrite(BlueLED, LOW);       // turn the Blue LED off 
  delay(100);

if (LoopDisplay >= LoopLog){

  delay(500); // Delay before returning back to the beginning of the procedure


  // ***************************   RTC and populate the time date variable ************************
// **********************************************************************************************

  //—–This part reads the time and disables the RTC alarm
    DateTime now = RTC.now(); //this reads the time from the RTC
    delay(10);

String(now.day()).toCharArray(b,3); // Convert the day number to a string and then add it to the temp char array b (2 digits + \0 char)
  if (now.day()<10)                 // if the number is less than 10 i.e. one digit, add a '0' in front of it
                                    // TimeDate_Tag[DD/MM/YYYY HH:MM:SS\0] 
    {
      TimeDate_Tag[0] = '0';        // [0]=D 1st digit as 0 if the number is only one digit i.e. 0 to 9
      TimeDate_Tag[1] = b[0];       // [1]=D 2nd digit as the number
    }
  else
    {
      TimeDate_Tag[0] = b[0];       // [0]=D 1st digit number
      TimeDate_Tag[1] = b[1];       // [1]=D 2nd digit number
    }
  TimeDate_Tag[2] = '/';            // [2]=/ Put a '/' date separator


// Repeat the day procedure for month
String(now.month()).toCharArray(b,3);  // Convert the Month number to a string and then add it to the temp char array b (2 digits + \0 char)
  if (now.month()<10) 
    {
      TimeDate_Tag[3] = '0';        // [3]=M 0 if one digit 
      TimeDate_Tag[4] = b[0];       // [4]=M 
    }
  else
    {
      TimeDate_Tag[3] = b[0];       // [3]=M 
      TimeDate_Tag[4] = b[1];       // [4]=M 
    }
  TimeDate_Tag[5] = '/';           // [5]=/ Put a '/' date separator


// Repeat the day procedure for year
String(now.year()).toCharArray(b,5); // Convert the Year number to a string and then add it to the temp char array b (4 digits + \0 char)
    TimeDate_Tag[6] = b[0];        // [6]=Y
    TimeDate_Tag[7] = b[1];        // [7]=Y
    TimeDate_Tag[8] = b[2];        // [8]=Y
    TimeDate_Tag[9] = b[3];        // [9]=Y
    TimeDate_Tag[10] = ' ';        // [10]=' ' Put a ' ' to separator date and time


// Repeat the day procedure for hours
String(now.hour()).toCharArray(b,3);  // Convert the Hour number to a string and then add it to the temp char array b (2 digits + \0 char)
  if (now.hour()<10) 
    {
      TimeDate_Tag[11] = '0';        // [11]=H 0 if one digit
      TimeDate_Tag[12] = b[0];       // [12]=H
    }
  else
    {
      TimeDate_Tag[11] = b[0];       // [11]=H
      TimeDate_Tag[12] = b[1];       // [12]=H
    }
  TimeDate_Tag[13] = ':';            // [13]=/ Put a ':' Time separator


// Repeat the day procedure for minutes
String(now.minute()).toCharArray(b,3);   // Convert the Minute number to a string and then add it to the temp char array b (2 digits + \0 char)
  if (now.minute()<10) 
    {
      TimeDate_Tag[14] = '0';       // [14]=M 0 if one digit
      TimeDate_Tag[15] = b[0];      // [15]=M
    }
  else
    {
      TimeDate_Tag[14] = b[0];      // [14]=M
      TimeDate_Tag[15] = b[1];      // [15]=M
    }
  TimeDate_Tag[16] = ':';            // [16]=/ Put a ':' Time separator


// Repeat the day procedure for seconds
String(now.second()).toCharArray(b,3);   // Convert the Seconds number to a string and then add it to the temp char array b (2 digits + \0 char)
  if (now.second()<10) 
    {
      TimeDate_Tag[17] = '0';       // [17]=S 0 if one digit
      TimeDate_Tag[18] = b[0];      // [18]=S
    }
  else
    {
      TimeDate_Tag[17] = b[0];     // [17]=S
      TimeDate_Tag[18] = b[1];     // [18]=S
    }
  TimeDate_Tag[19] = '\0';         // [19]=Null add the NULL

// Populate the Date_Tag value
  for (int i = 0; i<8; i++) //  Date is the first 10 digits of the TimeDate_Tag string, but we read 8 and replace the last two digit with the year
       {
        Date_Tag[i] = TimeDate_Tag[i];   // re-create variable with the new Charactor Date [0 to 7 plus Null] (0 to 7 of TimeDate_Tag plus null)
       }
        Date_Tag[6] = TimeDate_Tag[8];  // Replace the year with just two digits year (this is currently 20)
        Date_Tag[7] = TimeDate_Tag[9];  // Replace the year with just two digits year (this is currently 20)

       Date_Tag[8] = '\0';  // add Null


// Populate the Date_Tag value
  for (int i = 11; i<19; i++)           // ignore the seconds by reading to digit 16 of the TimeDate_Tag
       {
        Time_Tag[i-11] = TimeDate_Tag[i];   // re-create variable with the new Charactor Time [0 to 7 plus Null] (11 to 18 of TimeDate_Tag plus null)
       }
       Time_Tag[8] = '\0';  // add Null


   if (now.year()<2017) 
   {
    strcpy(ErrorMessage,"Clock Battery");
    error(ErrorMessage);
    delay(3000);
   }

  delay(10);


 // read the RTC temperature register and print that out
    // Note: the DS3231 temp registers (11h-12h) are only updated every 64seconds
    Wire.beginTransmission(DS3231_I2C_ADDRESS);
    Wire.write(0x11);                     //the register where the temp data is stored
    Wire.endTransmission();
    Wire.requestFrom(DS3231_I2C_ADDRESS, 2);   //ask for two bytes of data
    if (Wire.available()) {
    tMSB = Wire.read();            //2’s complement int portion
    tLSB = Wire.read();             //fraction portion
    temp3231 = ((((short)tMSB << 8) | (short)tLSB) >> 6) /4.0;  // Allows for readings below freezing: thanks to Coding Badly
    temp3231 =temp3231 -2; // caliboration 2 degrees out 
   }
  else {
    temp3231 = 0;
    //if temp3231 contains zero, then you know you had a problem reading the data from the RTC!
  }



// Switch the load off before reading the voltages to log
  digitalWrite(B1DischargePin, HIGH); // the NO pins are connected to High in this relay module
  digitalWrite(B2DischargePin, HIGH); // the NO pins are connected to High in this relay module
  delay (10000); // wait 10 second5 to settle the battries before measurements

// turn the Blue LED on and off
  digitalWrite(BlueLED, HIGH);
  delay(100);
  digitalWrite(BlueLED, LOW);
  delay(100);
  digitalWrite(BlueLED, HIGH);
  delay(100);
  digitalWrite(BlueLED, LOW);
  delay(100);

// read the Battery values again as the load is off and the readings would be different from the displayed

  V1 = analogRead(B1Pin);  // read the input pin
  V2 = analogRead(B2Pin);  // read the input pin

  V1 = (double)(V1*16.05/1024) * V1Cal;  // 16.00 is equivilant to 5V (13.08==4.09) 
  V2 = (double)(V2*28.08/1024) * V2Cal;   // 28.01 is equivalant to 5v (26.22==4.68)
  B2 = (V2 - V1);


// Create String variable from floating numbers
  dtostrf(V1, 4, 2, V1Char);  //mininum width 4, precision 2 ; float value is copied onto buff
  dtostrf(V2, 4, 2, V2Char);  //mininum width 4, precision 2 ; float value is copied onto buff
  dtostrf(B2, 4, 2, B2Char);  //mininum width 4, precision 2 ; float value is copied onto buff
 
  strcpy(Line1,Time_Tag); 
  strcat(Line1," "); 
  strcat(Line1,Date_Tag);  
  sprintf(Line2, "V1: %s  V2: %s", V1Char, V2Char); 
  sprintf(Line3, "B1: %s  B2: %s", V1Char, B2Char);
  DisplayText(Line1, Line2, Line3);


// Write Date, Time, B1(V1),V2, B2, Temperature to the SD Card
  logfile.print(Date_Tag);   
  logfile.print(',');      
  logfile.print(Time_Tag);   
  logfile.print(',');   
  logfile.print(V1Char);
  logfile.print(',');   
  logfile.print(B1Latch);
  logfile.print(',');  
  logfile.print(V2Char);
  logfile.print(',');    
  logfile.print(B2Char);
  logfile.print(',');   
  logfile.print(B2Latch);
  logfile.print(',');   
  logfile.print(SafeThresholdChar);
  logfile.print(',');   
  logfile.print(temp3231);

  logfile.println(); // CVS file Next line
    

// empty the SD Write buffer by reading data      
  logfile.flush();

  delay(500); // Delay before restarting the system


//Switch the load back on if the threshold is not reached

// Need to re-write fully

// Set Discharge relays if the voltage is above safe discharge voltage
if (V1 <= SafeThreshold) 
    {
      B1Latch =0; // Latch relay to off position
      digitalWrite(B1DischargePin, HIGH); //Relay is triggered if no volatge is applied
    }
if (V1 > SafeThreshold || B1Latch > 0) {digitalWrite(B1DischargePin, LOW);}

if (B2 <= SafeThreshold) 
    {
      B2Latch =0; // Latch relay to off position
      digitalWrite(B2DischargePin, HIGH); //Relay is triggered if no volatge is applied
    }
if (B1 > SafeThreshold || B2Latch > 0) {digitalWrite(B2DischargePin, LOW);}



  //Flash Three times to indicate logged
  digitalWrite(BlueLED, HIGH);
  delay(100);
  digitalWrite(BlueLED, LOW);
  delay(100);
  digitalWrite(BlueLED, HIGH);
  delay(100);
  digitalWrite(BlueLED, LOW);
  delay(100);
  digitalWrite(BlueLED, HIGH);
  delay(100);
  digitalWrite(BlueLED, LOW);
  delay(100);

  strcpy(Line1,"Safe: ");
  strcat(Line1,SafeThresholdChar);
  strcat(Line1," ");
  
  LoopDisplay =0;
  delay(2000); 

}
  LoopDisplay =LoopDisplay + 1;
  delay(2000); // Loop every 2 seconds

} // end of void loop(void)
// ------------------------------------------------------------------------------ //


