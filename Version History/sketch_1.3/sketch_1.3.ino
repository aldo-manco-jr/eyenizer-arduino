/*
 * Aldo Jr.
 */
 
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal.h>
 
#define BACKLIGHT 2
#define CONTRAST  3
#define SET       A0
#define PIU       A1
#define MENO      A2
 
#define SS_PIN 53
#define RST_PIN 5
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

char buffer[10];
unsigned long time=0;
unsigned long timeSet=0;
int  setModeTime=2000;
 
RTC_DS1307 RTC;
LiquidCrystal lcd(12, 11, 7, 6, 5, 4);
 
void setup() 
{
  Serial.begin(9600);   // Initiate a serial communication
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  Serial.println("Approximate your card to the reader...");
  Serial.println();
  pinMode( BACKLIGHT, OUTPUT );
    pinMode( CONTRAST, OUTPUT );
    digitalWrite( BACKLIGHT, HIGH );
    analogWrite( CONTRAST, 40 );
 
    Wire.begin();
    RTC.begin();
    lcd.begin(16, 2);
 
    if (! RTC.isrunning()) {
      Serial.println("RTC is NOT running!");
      RTC.adjust(DateTime(__DATE__, __TIME__));
    }
 
    pinMode( SET, INPUT );
    pinMode( PIU, INPUT );
    pinMode( MENO, INPUT );
}

void setMode( DateTime now ) {
    boolean setMode = true;
    int setModeLevel = 0;
 
    int _day = now.day();
    int _month = now.month();
    int _year = now.year();
    int _hour = now.hour();
    int _min = now.minute();
    int _sec = now.second();

    Serial.println(_day + _month + _year + _hour + _min + _sec);
 
    lcd.clear();
    lcd.setCursor(0,0);
    sprintf(buffer,  "%s: %02d", "Giorno", _day);
    delay( 1000 );
    timeSet = millis();
 
    while ( setMode ) {
      if ( analogRead(SET) > 1000 || analogRead(PIU) > 1000 || analogRead(MENO) > 1000 ) { timeSet = millis(); }
 
      lcd.setCursor(0,0);
 
      // Set Day
      if ( setModeLevel == 0 ) {
        if ( analogRead(PIU) > 1000 && _day < 31) { _day++; }
        if ( analogRead(MENO) > 1000 && _day > 1) { _day--; }
 
        sprintf(buffer,  "%s: %02d", "Giorno", _day);
      }
      // Set Month
      if ( setModeLevel == 1 ) {
        if ( analogRead(PIU) > 1000 && _month < 12) { _month++; }
        if ( analogRead(MENO) > 1000 && _month > 1) { _month--; }
 
        sprintf(buffer,  "%s: %02d", "Mese", _month);
      }
      // Set Year
      if ( setModeLevel == 2 ) {
        if ( analogRead(PIU) > 1000 && _year < 9999) { _year++; }
        if ( analogRead(MENO) > 1000 && _year > 1900) { _year--; }
 
        sprintf(buffer,  "%s: %02d", "Anno", _year);
      }
      // Set Hour
      if ( setModeLevel == 3 ) {
        if ( analogRead(PIU) > 1000 && _hour < 24) { _hour++; }
        if ( analogRead(MENO) > 1000 && _hour > 1) { _hour--; }
 
        sprintf(buffer,  "%s: %02d", "Ora", _hour);
      }
      // Set Minute
      if ( setModeLevel == 4 ) {
        if ( analogRead(PIU) > 1000 && _min < 60) { _min++; }
        if ( analogRead(MENO) > 1000 && _min > 1) { _min--; }
 
        sprintf(buffer,  "%s: %02d", "Minuti", _min);
      }
      // Set Second
      if ( setModeLevel == 5 ) {
        if ( analogRead(PIU) > 1000 && _sec < 60) { _sec++; }
        if ( analogRead(MENO) > 1000 && _sec > 0) { _sec--; }
 
        sprintf(buffer,  "%s: %02d", "Secondi", _sec);
      }
 
      lcd.print( buffer );
      if ( analogRead(SET) > 1000 ) { lcd.clear(); setModeLevel++;  }
      if ( setModeLevel > 5 ) { setModeLevel=0; }
 
      if (timeSet > 0 && (setModeTime*2) < (millis() - timeSet) ) {
         RTC.adjust(DateTime(_year, _month, _day, _hour, _min, _sec));
         setMode = false;
      }
      delay(200);
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
DateTime now = RTC.now();
 
    Serial.print(now.day(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.year(), DEC);
    Serial.print(' ');
    if(now.hour()+14>23){
      Serial.print((now.hour()+14)-24, DEC);
    }else{
      Serial.print(now.hour()+14, DEC);
    }
    Serial.print(':');
    if(now.second()>56){
      if(now.minute()-3<0){
      Serial.print((now.minute()-3)+60, DEC);
    }else{
      Serial.print(now.minute()-3, DEC); 
    }
    }else{
      if(now.minute()-3<0){
      Serial.print((now.minute()-3)+59, DEC);
    }else{
      Serial.print(now.minute()-4, DEC); 
    }
    }
    
    Serial.print(':');
    if(now.second()-24<0){
      Serial.print((now.second()-24)+60, DEC);
    }else{
      Serial.print(now.second()-24, DEC);
    }
 
    Serial.println();
    delay(3000);
}
