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
 
#define SS_PIN 53
#define RST_PIN 3
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
 
void setup() 
{
  Serial.begin(9600);   // Initiate a serial communication
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  Serial.println("Approximate your card to the reader...");
  Serial.println();

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
    Serial.println();
    delay(3000);
  }
  else if (content.substring(1) == "C6 18 26 9E") //change here the UID of the card/cards that you want to give access
  {
    Serial.println("Luigi Santagata");
    Serial.println();
    delay(3000);
  }
 
 else   {
    Serial.println("Scheda non valida");
    delay(3000);
  }
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
