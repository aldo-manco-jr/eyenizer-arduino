/* @file HelloKeypad.pde
|| @version 1.0
|| @author Alexander Brevig
|| @contact alexanderbrevig@gmail.com
||
|| @description
|| | Demonstrates the simplest use of the matrix Keypad library.
|| #
*/
#include <SPI.h>
#include <MFRC522.h>
#include "Wire.h"
#include <SD.h>
#include "LiquidCrystal.h"
#include <Keypad.h>
 
const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
  {'#','0','*'},
  {'9','8','7'},
  {'6','5','4'},
  {'3','2','1'}
};
byte rowPins[ROWS] = {28,27,26,25}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {24,23,22}; //connect to the column pinouts of the keypad
 
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
 
void setup(){
  Serial.begin(9600);
    SPI.begin();
  lcd.begin(16,2);
  lcd.setCursor(0,0);
}
 
void loop(){
  char key = keypad.getKey();
 
  if(key){
    lcd.print(key);
  }
}
  

