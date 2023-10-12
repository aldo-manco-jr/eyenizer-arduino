#include "LiquidCrystal.h"

// initialize the library by providing the nuber of pins to it
LiquidCrystal lcd(8,9,4,5,6,7);

void setup() {
lcd.begin(16,2);

// set cursor position to start of first line on the LCD
lcd.setCursor(0,0);
//text to print
lcd.print("    COUNTER");
delay(100);

int a=0;
lcd.setCursor(0,1);
lcd.print("       ");
lcd.print(a);
while(a<=100)
{
  a=a+1;
  if(a%3==0){
    
  delay(1000);
  lcd.setCursor(0,1);
  lcd.print("       ");
  lcd.print(a);
  }
}
}
void loop()
{
  lcd.clear();
}
