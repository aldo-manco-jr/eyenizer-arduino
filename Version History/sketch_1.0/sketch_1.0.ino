/*
 * 
 * All the resources for this project: http://randomnerdtutorials.com/
 * Modified by Rui Santos
 * 
 * Created by FILIPEFLOP
 * 
 */
 
#include <SPI.h>
#include <MFRC522.h>
#include "Wire.h"

#define DS3231_I2C_ADDRESS 0x68
// Convert normal decimal numbers to binary coded decimal
#define SS_PIN 53
#define RST_PIN 5
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

byte decToBcd(byte val)
{
  return( (val/10*16) + (val%10) );
}
// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val)
{
  return( (val/16*10) + (val%16) );
}
 
void setup() 
{
  Serial.begin(9600);   // Initiate a serial communication
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  Serial.println("Approximate your card to the reader...");
  Serial.println();
  Wire.begin();
  // set the initial time here:
  // DS3231 seconds, minutes, hours, day, date, month, year
  // setDS3231time(30,42,21,4,26,11,14);
}

void setDS3231time(byte second, byte minute, byte hour, byte dayOfWeek, byte
dayOfMonth, byte month, byte year)
{
  // sets time and date data to DS3231
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set next input to start at the seconds register
  Wire.write(decToBcd(second)); // set seconds
  Wire.write(decToBcd(minute)); // set minutes
  Wire.write(decToBcd(hour)); // set hours
  Wire.write(decToBcd(dayOfWeek)); // set day of week (1=Sunday, 7=Saturday)
  Wire.write(decToBcd(dayOfMonth)); // set date (1 to 31)
  Wire.write(decToBcd(month)); // set month
  Wire.write(decToBcd(year)); // set year (0 to 99)
  Wire.endTransmission();
}
void readDS3231time(byte *second,
byte *minute,
byte *hour,
byte *dayOfWeek,
byte *dayOfMonth,
byte *month,
byte *year)
{
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set DS3231 register pointer to 00h
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
  // request seven bytes of data from DS3231 starting from register 00h
  *second = bcdToDec(Wire.read() & 0x7f);
  *minute = bcdToDec(Wire.read());
  *hour = bcdToDec(Wire.read() & 0x3f);
  *dayOfWeek = bcdToDec(Wire.read());
  *dayOfMonth = bcdToDec(Wire.read());
  *month = bcdToDec(Wire.read());
  *year = bcdToDec(Wire.read());
}
void displayTime()
{
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  // retrieve data from DS3231
  readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month,
  &year);
  // send it to the serial monitor
  Serial.print(hour, DEC);
  // convert the byte variable to a decimal number when displayed
  Serial.print(":");
  if (minute<10)
  {
    Serial.print("0");
  }
  Serial.print(minute, DEC);
  Serial.print(":");
  if (second<10)
  {
    Serial.print("0");
  }
  Serial.print(second, DEC);
  Serial.print(" ");
  Serial.print(dayOfMonth, DEC);
  Serial.print("/");
  Serial.print(month, DEC);
  Serial.print("/");
  Serial.print(year, DEC);
  Serial.print(" Day of week: ");
  switch(dayOfWeek){
  case 1:
    Serial.println("Sunday");
    break;
  case 2:
    Serial.println("Monday");
    break;
  case 3:
    Serial.println("Tuesday");
    break;
  case 4:
    Serial.println("Wednesday");
    break;
  case 5:
    Serial.println("Thursday");
    break;
  case 6:
    Serial.println("Friday");
    break;
  case 7:
    Serial.println("Saturday");
    break;
  }
}

void loop() 
{
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  //Show UID on serial monitor
  Serial.print("UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  if (content.substring(1) == "F4 D1 04 B0") //change here the UID of the card/cards that you want to give access
  {
    Serial.println("Eduardo Manco");
    delay(3000);
  }
  else if (content.substring(1) == "C6 18 26 9E") //change here the UID of the card/cards that you want to give access
  {
    Serial.println("Luigi Santagata");
    delay(3000);
  }
 
 else   {
    Serial.println("Scheda non valida");
    delay(3000);
  }
  displayTime(); // display the real-time clock data on the Serial Monitor,
  delay(1000); // every second
  Serial.println();
} 

/*
 * /*
 * RTC & LCD
 *
 * Autore: Mauro Alfieri
 * Tw: @mauroalfieri
 *
 * Web: http://www.mauroalfieri.it
 
#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal.h>
 
#define BACKLIGHT 2
#define CONTRAST  3
 
char buffer[10];
 
RTC_DS1307 RTC;
LiquidCrystal lcd(12, 11, 7, 6, 5, 4);
 
void setup () {
    pinMode( BACKLIGHT, OUTPUT );
    pinMode( CONTRAST, OUTPUT );
    digitalWrite( BACKLIGHT, HIGH );
    analogWrite( CONTRAST, 40 );
 
    Wire.begin();
    RTC.begin();
    lcd.begin(16, 2);
 
    RTC.sqw(1);
    if (! RTC.isrunning()) {
      RTC.adjust(DateTime(__DATE__, __TIME__));
    }
}
 
void loop () {
 
    DateTime now = RTC.now();
    lcd.clear();
 
    sprintf(buffer,  "%02d/%02d/%d", now.day(), now.month(), now.year());
    lcd.setCursor(0,0);
    lcd.print( buffer );
 
    char buffer[10] = "";
 
    sprintf(buffer,  "%02d:%02d:%02d", now.hour(), now.minute(), now.second());
    lcd.setCursor(0,1);
    lcd.print( buffer );
 
    delay(1000);
}
*/
