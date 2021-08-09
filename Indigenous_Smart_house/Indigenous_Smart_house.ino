/***************************************************
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

// RTC
#include "RTClib.h" // Includes the RTC library  

RTC_PCF8523 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"}; // Stores the days of the week as character values

// SD Card Adalogger
#include "FS.h" // Enables read/write access to the SD card on the Adalogger
#include "SD.h" // Enables read/write access to the SD card on the Adalogger
#include <Adafruit_MotorShield.h> // includes the adafruit motor shield library

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); // a reference to the library being used
Adafruit_DCMotor *myMotor = AFMS.getMotor(4); // a reference to the library being used

// EINK
#include "Adafruit_ThinkInk.h" // includes the adafruit ThinkInk.h library
// defined variables
#define EPD_CS      15 
#define EPD_DC      33
#define SRAM_CS     32
#define EPD_RESET   -1 // can set to -1 and share with microcontroller Reset!
#define EPD_BUSY    -1 // can set to -1 to not use a pin (will wait a fixed delay)

// 2.13" Monochrome displays with 250x122 pixels and SSD1675 chipset
ThinkInk_213_Mono_B72 display(EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY);

// moisture sensor
int moistureValue = 0; //value for storing moisture value
int soilPin = 12;//Declare a variable for the soil moisture sensor
// int soilPower = 7;//Variable for Soil moisture Power

//Rather than powering the sensor through the 3.3V or 5V pins,
//we'll use a digital pin to power the sensor. This will
//prevent corrosion of the sensor as it sits in the soil.

void setup() {
  Serial.begin(9600); // sets the data rate
  while (!Serial) { // indicates if the specified serial port is ready
    delay(10); // delays for one milisecond 
  }

  // RTC
  if (! rtc.begin()) { // Checks if the RTC has been initialised 
    Serial.println("Couldn't find RTC"); // prints couldn't find RTC in the serial monitor
    Serial.flush(); // Waits for the transmission of outgoing serial data to complete
    abort(); // termintate the program
  }

  // The following line can be uncommented if the time needs to be reset.
  //  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  rtc.start();

  //EINK
  display.begin(THINKINK_MONO); // begins to display on the ThinkInk in monochrome
  display.clearBuffer(); // clears the buffer on the display




  if (!SD.begin()) { // Checks if the SD library and cardhave been initialized
    Serial.println("Card Mount Failed"); // prints card mount failed
    return; // returns the value
  }
  uint8_t cardType = SD.cardType(); // gets the SD card type

  if (cardType == CARD_NONE) { // Checks if there is an SD card being used
    Serial.println("No SD card attached"); // Prints no SD card attached to the serial monitor
    return; // returns the value 
  }
  Serial.println("SD Started"); // prints SD card started to the serial monitor

  AFMS.begin(); // starts the AFMS motor shield
  myMotor->setSpeed(255); // sets the motor speed
  logEvent("System Initialisation..."); // logs the event 
}

void loop() {

  // Gets the current date and time, and writes it to the Eink display.
  String currentTime = getDateTimeAsString();

  drawText("The Current Time and\nDate is", EPD_BLACK, 2, 0, 0); // draws the current date and time on the Eink in black with dimensions 2,0,0

  // writes the current time on the bottom half of the display (y is height)
  drawText(currentTime, EPD_BLACK, 2, 0, 75);

  // Draws a line from the leftmost pixel, on line 50, to the rightmost pixel (250) on line 50.
  display.drawLine(0, 50, 250, 50, EPD_BLACK);


  int moisture = readSoil(); // reads the integer soil moisture
  drawText(String(moisture), EPD_BLACK, 2, 0, 100); // draws the current moisture value on the Eink in black with the dimensions 2,0,100
  display.display(); // displays the above text
  // waits 180 seconds (3 minutes) as per guidelines from adafruit.
  delay(180000);
  display.clearBuffer(); // clears the display buffer

  Serial.print("Soil Moisture = "); // prints the soil moisture on the serial monitor
  //get soil moisture value from the function below and print it
  Serial.println(readSoil());
}

void drawText(String text, uint16_t color, int textSize, int x, int y) { // Draws the text in a monochrome colour with the correct x and y coordinates
  display.setCursor(x, y); // displays the cursur at x and y
  display.setTextColor(color); // displays the chosen colour
  display.setTextSize(textSize); // displays the chosen text size
  display.setTextWrap(true); // sets text wrap to true
  display.print(text); // prints the text with the above values

}

String getDateTimeAsString() {
  DateTime now = rtc.now(); // gets the current date on the real time clock

  //Prints the date and time to the Serial monitor for debugging.
  /*
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
  */

  // Converts the date and time into a human-readable format.
  char humanReadableDate[20];
  sprintf(humanReadableDate, "%02d:%02d:%02d %02d/%02d/%02d",  now.hour(), now.minute(), now.second(), now.day(), now.month(), now.year());

  return humanReadableDate; // returns the readable date 
}

void logEvent(String dataToLog) {
  /*
     Log entries to a file on an SD card.
  */
  // Get the updated/current time
  DateTime rightNow = rtc.now();

  // Open the log file
  File logFile = SD.open("/logEvents.csv", FILE_APPEND);
  if (!logFile) {
    Serial.print("Couldn't create log file");
    abort();
  }

  // Log the event with the date, time and data
  logFile.print(rightNow.year(), DEC);
  logFile.print(",");
  logFile.print(rightNow.month(), DEC);
  logFile.print(",");
  logFile.print(rightNow.day(), DEC);
  logFile.print(",");
  logFile.print(rightNow.hour(), DEC);
  logFile.print(",");
  logFile.print(rightNow.minute(), DEC);
  logFile.print(",");
  logFile.print(rightNow.second(), DEC);
  logFile.print(",");
  logFile.print(dataToLog);

  // End the line with a return character.
  logFile.println();
  logFile.close();
  Serial.print("Event Logged: ");
  Serial.print(rightNow.year(), DEC);
  Serial.print(",");
  Serial.print(rightNow.month(), DEC);
  Serial.print(",");
  Serial.print(rightNow.day(), DEC);
  Serial.print(",");
  Serial.print(rightNow.hour(), DEC);
  Serial.print(",");
  Serial.print(rightNow.minute(), DEC);
  Serial.print(",");
  Serial.print(rightNow.second(), DEC);
  Serial.print(",");
  Serial.println(dataToLog);
}

//This is a function used to get the soil moisture content
int readSoil()
{
  moistureValue = analogRead(soilPin);//Read the SIG value form sensor
  return moistureValue;//send current moisture value
}
