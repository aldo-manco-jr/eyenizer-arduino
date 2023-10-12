/*
 * Logbook 1.4 - Aldo Jr
 */
#include <SPI.h>
#include <MFRC522.h>
#include "Wire.h"
#include <SD.h>

#define DS3231_I2C_ADDRESS 0x68
#define SS_PIN 53
#define RST_PIN 3
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.



byte decToBcd(byte val)
{
  return( (val/10*16) + (val%10) );
}



byte bcdToDec(byte val)
{
  return( (val/16*10) + (val%16) );
}


 
void setup() 
{
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println("Avvicina la tua tessera al lettore...");
  Serial.println();
  Wire.begin();
  // secondi, minuti, ore, settimanale, giorno, mese, anno
  setDS3231time(0,0,21,4,28,12,16);
  Serial.begin(9600);
  Serial.print("Inizializzazione Card: ");
  if (!SD.begin(2)) //CS==4
  {
    Serial.println("FALLITA!");
    return;
  }
  Serial.println("ESEGUITO!");
}



void setDS3231time(
  byte second,
  byte minute, 
  byte hour, 
  byte dayOfWeek, 
  byte dayOfMonth, 
  byte month, 
  byte year)
{
  // Carica la data su DS3231
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



void readDS3231time(
  byte *second,
  byte *minute,
  byte *hour,
  byte *dayOfWeek,
  byte *dayOfMonth,
  byte *month,
  byte *year)
{
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // lancetta su mezzanotte
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
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


  
  if (content.substring(1) == "F4 D1 04 B0")
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
    eduardo.print("-");
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


  
  else if (content.substring(1) == "C6 18 26 9E")
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
    luigi.print("-");
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
    invalid.print("-");
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
