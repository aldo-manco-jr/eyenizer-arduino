/*
 * Superuser 1.6 - Aldo & Eduardo Manco
 * Recorder of Entrances and Exits of Employees From Building
 * Platform: Arduino/Genuino Mega or Mega 2560
 */
 
#include <SPI.h>
#include <MFRC522.h>
#include "Wire.h"
#include <SD.h>
#include "LiquidCrystal.h"

#define DS3231_I2C_ADDRESS 0x68
#define SDA_PIN 53
#define RST_PIN 3
#define GREEN 12
#define RED 13
MFRC522 mfrc522(SDA_PIN, RST_PIN);
LiquidCrystal lcd(8,9,4,5,6,7);

// Convertire Numero Decimale in Numero Binario

byte decToBin(byte number)
{
  return((number/10*16)+(number%10));
}

// Convertire Numero Binario in Numero Decimale

byte binToDec(byte number)
{
  return((number/16*10)+(number%16));
}


 
void setup() 
{
  // Inizializza Comunicazione Seriale
  Serial.begin(9600);
  // Inizializza SPI BUS
  SPI.begin();
  //Inizializza RFID MFRC522
  mfrc522.PCD_Init();
  // Inizializza Comunicazione tra Arduino & RTC
  Wire.begin();
  // Inizializza Data & Ora
  // Secondi, Minuti, Ore, Giorno Settimanale, Giorno, Mese, Anno
  setter(0,0,8,4,28,12,16);
  // Inizializza Schermo LCD a 2 Righe con 16 Caratteri 
  lcd.begin(16,2);
  // Sposta il Cursore sulla Parte Superiore dello Schermo LCD
  lcd.setCursor(0,0);
  pinMode(GREEN, OUTPUT);
  pinMode(RED, OUTPUT);
  if(!SD.begin(2)) //Se il PIN CS Non è Impostato su 2
  {
    return;
  }
}

// Inizializzazione Data & Ora su RTC

void setter(
  byte second,
  byte minute, 
  byte hour, 
  byte dayOfWeek, 
  byte dayOfMonth, 
  byte month, 
  byte year)
{
  // Inizia Comunicazione con RTC DS3231
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  // Imposta Input Successivo Per Iniziare a Registrare i Secondi
  Wire.write(0);
  // Inizializza i Parametri di Data & Ora con Sistema Decimale, li Trasforma in Numero Binario e li Immagazzina in 7 Byte sull'RTC
  Wire.write(decToBin(second)); // Inizializza Secondi (0, 60)
  Wire.write(decToBin(minute)); // Inizializza Minuti (0, 60)
  Wire.write(decToBin(hour)); // Inizializza Ore (0, 24)
  Wire.write(decToBin(dayOfWeek)); // Inizializza Giorno della Settimana (1, 7)
  Wire.write(decToBin(dayOfMonth)); // Inizializza Giorno (1, 31)
  Wire.write(decToBin(month)); // Inizializza Mese (1, 12)
  Wire.write(decToBin(year)); // Inizializza Anno (0, 99)
  // Fine Comunicazione con RTC DS3231
  Wire.endTransmission();
}

// Lettura Data & Ora da RTC

void getter(
  byte *second,
  byte *minute,
  byte *hour,
  byte *dayOfWeek,
  byte *dayOfMonth,
  byte *month,
  byte *year)
{
  // Inizia Comunicazione con RTC DS3231
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  // Imposta Input Successivo Per Iniziare a Registrare i Secondi
  Wire.write(0);
  // Fine Comunicazione con RTC DS3231
  Wire.endTransmission();
  // Ottenere dall'RTC i 7 Byte Contenenti Data & Ora con Sistema Binario e Trasformarli in Numero Decimale
  Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
  // Leggi Data & Ora su RTC con Sistema Decimale
  *second = binToDec(Wire.read() & 0x7f); // Leggi Secondo
  *minute = binToDec(Wire.read()); // Leggi Minuto
  *hour = binToDec(Wire.read() & 0x3f); // Leggi Ora
  *dayOfWeek = binToDec(Wire.read()); // Leggi Giorno della Settimana
  *dayOfMonth = binToDec(Wire.read()); // Leggi Giorno
  *month = binToDec(Wire.read()); // Leggi Mese
  *year = binToDec(Wire.read()); // Leggi Anno
}

// Antepone uno Zero se i Numeri vanno da 0 a 9

void zero(int number){
  if(number<10 && number>-1){
    lcd.print("0");
  }
}



void loop() 
{
  if(!mfrc522.PICC_IsNewCardPresent()) // Se Non Viene Letto Nessun Badge 
  {
    return;
  }
  if(!mfrc522.PICC_ReadCardSerial()) // Se il UID Tag del Badge Non Viene Letto 
  {
    return;
  }
  
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  // Leggi Data & Ora da RTC
  getter(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);
  
  String badge = ""; //Stringa Vuota
  // Concatena UID Tag del Badge alla Stringa Vuota
  for(byte i=0; i<mfrc522.uid.size; i++) 
  {
     badge.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     badge.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  // Rendi Lettere Esadecimali della Variabile 'badge' Contenente il UID Tag Maiuscole
  badge.toUpperCase();


  //Se la Variabile Contiene il UID Tag del Badge di Eduardo Manco
  if (badge.substring(1) == "F4 D1 04 B0")
  {
    // Crea File Per Ogni Dipendente
    File eduardo;
    // Apri File Per Scrittura
    eduardo = SD.open("eduardo.txt", FILE_WRITE);
    // Scrive Data & Ora sul File nel Seguente Formato: dd/mm/yyyy - 23:59:59
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
    // Riattivare Lettura fra 1 Secondo
    delay(1000);
    eduardo.println();
    // Chiudi File del Dipendente
    eduardo.close();
    digitalWrite(GREEN, HIGH);
    // Sposta il Cursore sulla Parte Superiore dello Schermo LCD
    lcd.setCursor(0,0);
    // Stampa il Nome del Dipendente che ha Appena Strisciato il Badge
    lcd.print("Eduardo Manco");
    // Sposta il Cursore sulla Parte Inferiore dello Schermo LCD
    lcd.setCursor(0,1);
    // Sposta il Cursore al Centro
    lcd.print("     ");
    // Anteponi uno Zero se l'Ora Corrente è Compresa tra 0 e 9
    zero(hour);
    // Stampa l'Orario
    lcd.print(hour);
    lcd.print(":");
    // Anteponi uno Zero se il Minuto Corrente è Compreso tra 0 e 9
    zero(minute);
    lcd.print(minute);
    // Dissolvi la Scritta Dopo 3 Secondi
    delay(3000);
    digitalWrite(GREEN, LOW);
    lcd.clear();
  }


  
  else if (badge.substring(1) == "C6 18 26 9E")
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
    delay(1000);
    luigi.println();
    luigi.close();
    digitalWrite(GREEN, HIGH);
    lcd.setCursor(0,0);
    lcd.print("Luigi Santagata");
    lcd.setCursor(0,1);
    lcd.print("     ");
    zero(hour);
    lcd.print(hour);
    lcd.print(":");
    zero(minute);
    lcd.print(minute);
    delay(3000);
    digitalWrite(GREEN, LOW);
    lcd.clear();
  }


 
  else //Se il UID Tag del Badge non è di Nessun Dipendente
  {
    File invalid; // Crea un File Per Tutti i Badge Non Validi
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
    delay(1000);
    invalid.println();
    invalid.close();
    digitalWrite(RED, HIGH);
    lcd.setCursor(0,0);
    lcd.print("Non Valida");
    lcd.setCursor(0,1);
    lcd.print("     ");
    zero(hour);
    lcd.print(hour);
    lcd.print(":");
    zero(minute);
    lcd.print(minute);
    delay(3000);
    digitalWrite(RED, LOW);
    lcd.clear();
  }
}
