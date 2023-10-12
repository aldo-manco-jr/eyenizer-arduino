#include <SPI.h>
#include <MFRC522.h>
#include "Wire.h"
#include <SD.h>

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
  setDS3231time(0,0,21,4,28,12,16);
  Serial.begin(9600);
  Serial.print("Inizializzazione Card: ");
  if (!SD.begin(4)) //il Pin 4 è collegato a CS
  {
    Serial.println("FALLITA!");
    return;
  }
  Serial.println("ESEGUITO!");
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



  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  // retrieve data from DS3231
  readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);
  
  //Show UID on serial monitor
  Serial.print("UID:");
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
  content.toUpperCase();


  
  if (content.substring(1) == "F4 D1 04 B0") //change here the UID of the card/cards that you want to give access
  {
    File eduardo;
    eduardo = SD.open("eduardo.txt", FILE_WRITE);
    eduardo.print(hour, DEC);
    eduardo.print(":");
    if (minute<10)
    {
      eduardo.print("0");
    }
    eduardo.print(minute, DEC);
    eduardo.print(" ");
    eduardo.print(dayOfMonth, DEC);
    eduardo.print("/");
    eduardo.print(month, DEC);
    eduardo.print("/");
    eduardo.print(year, DEC);
    delay(3000); // every second
    eduardo.println();
    eduardo.close();
  }


  
  else if (content.substring(1) == "C6 18 26 9E") //change here the UID of the card/cards that you want to give access
  {
    File luigi;
    luigi = SD.open("luigi.txt", FILE_WRITE);
    luigi.print(hour, DEC);
    luigi.print(":");
    if (minute<10)
    {
      luigi.print("0");
    }
    luigi.print(minute, DEC);
    luigi.print(" ");
    luigi.print(dayOfMonth, DEC);
    luigi.print("/");
    luigi.print(month, DEC);
    luigi.print("/");
    luigi.print(year, DEC);
    delay(3000); // every second
    luigi.println();
    luigi.close();
  }


 
  else   
  {
    File invalid;
    invalid = SD.open("invalid.txt", FILE_WRITE);
    invalid.print(hour, DEC);
    invalid.print(":");
    if (minute<10)
    {
      invalid.print("0");
    }
    invalid.print(minute, DEC);
    invalid.print(" ");
    invalid.print(dayOfMonth, DEC);
    invalid.print("/");
    invalid.print(month, DEC);
    invalid.print("/");
    invalid.print(year, DEC);
    delay(3000); // every second
    invalid.println();
    invalid.close();
  }
  
  delay(5000); //Attendi 5 secondi prima di ricominciare
}
