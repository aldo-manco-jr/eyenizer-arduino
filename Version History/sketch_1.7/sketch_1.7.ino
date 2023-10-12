/*
 * BigBrother 1.7 - Aldo & Eduardo Manco
 * Recorder of Entrances and Exits of Employees From Building
 * Platform: Arduino/Genuino Mega or Mega 2560
 */
 
#include <SPI.h>
#include <MFRC522.h>
#include "Wire.h"
#include <SD.h>
#include "LiquidCrystal.h"

#define SDA_PIN 53
#define RST_PIN 3
MFRC522 mfrc522(SDA_PIN, RST_PIN);

#define DS3231_I2C_ADDRESS 0x68

LiquidCrystal lcd(8,9,4,5,6,7);

#define GREEN 12
#define RED 13

int speaker = 11; // PIN -> 9|10|11
#define c 3830 // 261Hz 
#define d 3400 // 294Hz 
#define e 3038 // 329Hz 
#define f 2864 // 349Hz 
#define g 2550 // 392Hz 
#define a 2272 // 440Hz 
#define b 2028 // 493Hz 
#define C 1912 // 523Hz 

// Stampa sullo Schermo LCD le Istruzioni

void printInstruction(){
  lcd.print("Avvicina il");
  // Sposta il Cursore sulla Parte Inferiore dello Schermo LCD
  lcd.setCursor(0, 1);
  lcd.print("Tuo Badge :\u0029");
}

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

// Scrivi Data & Ora su File nel Formato: lunedì dd/mm/yyyy hh:mm

void datetime(
  File file, 
  int minute, 
  int hour, 
  int dayOfWeek, 
  int dayOfMonth, 
  int month, 
  int year)
{
  String week;
  // Scrivi Giorno Settimanale Come Testo
  switch(dayOfWeek){
    case 1: week = "Lunedi"; break;
    case 2: week = "Martedi"; break;
    case 3: week = "Mercoledi"; break;
    case 4: week = "Giovedi"; break;
    case 5: week = "Venerdi"; break;
    case 6: week = "Sabato"; break;
    case 7: week = "Domenica"; break;
  }
  file.print(week);
  file.print(" ");
  if(dayOfMonth<10 && dayOfMonth>-1){
    file.print("0");
  }
  file.print(dayOfMonth, DEC);
  file.print("/");
  if(month<10 && month>-1){
    file.print("0");
  }
  file.print(month, DEC);
  file.print("/");
  file.print(year, DEC);
  file.print(" ");
  if(hour<10 && hour>-1){
    file.print("0");
  }
  file.print(hour, DEC);
  file.print(":");
  if(minute<10 && minute>-1){
    file.print("0");
  }
  file.print(minute, DEC);
  delay(1000);
  // Scrivi Prossimo Spostamento a Capo
  file.println();
}

// Stampa Ora al Centro della Parte Inferiore dello Schermo LCD

void timeOnLCD(
  int hour, 
  int minute){
  // Sposta il Cursore sulla Parte Inferiore dello Schermo LCD
  lcd.setCursor(0,1);
  // Sposta il Cursore al Centro
  lcd.print("     ");
  // Anteponi uno Zero se l'Ora Corrente è Compresa tra 0 e 9
  if(hour<10 && hour>-1){
    lcd.print("0");
  }
  // Stampa l'Orario
  lcd.print(hour);
  lcd.print(":");
  // Anteponi uno Zero se il Minuto Corrente è Compreso tra 0 e 9
  if(minute<10 && minute>-1){
    lcd.print("0");
  }
  lcd.print(minute);
}

// Riproduci il Suono di una Nota per una Durata Prestabilita

void playNote(){
  // Durata della Nota
  int note = C;
  // Durata del Suono in Microsecondi
  long duration = 1000000;
  // Tempo Trascorso
  long elapsedTime = 0;

  // Suona Fino a Quando Non Raggiunge un Lasso di Tempo Prestabilito
  while(elapsedTime < duration)
  {
    // Inizia Suono
    digitalWrite(speaker, HIGH);
    // Ritardo tra un Suono e l'Altro
    delayMicroseconds(note/2);

    // Termina Suono
    digitalWrite(speaker, LOW);
    delayMicroseconds(note/2);

    // Calcola il Tempo Passato
    elapsedTime+=note;
  }
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
  // Inizializza Data & Ora (Secondi, Minuti, Ore, Giorno Settimanale, Giorno, Mese, Anno)
  setter(0,0,8,2,3,1,17);
  // Inizializza Schermo LCD a 2 Righe con 16 Caratteri 
  lcd.begin(16,2);
  // Sposta il Cursore sulla Parte Superiore dello Schermo LCD
  lcd.setCursor(0,0);
  // Inizializza LED Verde
  pinMode(GREEN, OUTPUT);
  // Inizializza LED Rosso
  pinMode(RED, OUTPUT);
  //inizializza Speaker
  pinMode(speaker, OUTPUT);
  if(!SD.begin(2)) //Se il PIN CS Non è Impostato su 2
  {
    return;
  }
  printInstruction();
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
    datetime(eduardo, minute, hour, dayOfWeek, dayOfMonth, month, year);
    // Chiudi File del Dipendente
    eduardo.close();
    // Cancella Scritta "Avvicina il Tuo Badge :)"
    lcd.clear();
    // Accendi LED Verde
    digitalWrite(GREEN, HIGH);
    // Stampa il Nome del Dipendente che ha Appena Strisciato il Badge
    lcd.print("Eduardo Manco");
    timeOnLCD(hour, minute);
    playNote();
    // Leggi gli Statement che Seguono Dopo 3 Secondi, 1 della Funzione playNote() + 2 di Ritardo
    delay(2000);
    // Spegni LED Verde
    digitalWrite(GREEN, LOW);
    // Pulisci Schermo LCD
    lcd.clear();
    // Riscrivi Istruzione Iniziale con Mezzo Minuto di Ritardo
    delay(500);
    printInstruction();
  }


  
  else if (badge.substring(1) == "C6 18 26 9F")
  {
    File luigi;
    luigi = SD.open("luigi.txt", FILE_WRITE);
    datetime(luigi, minute, hour, dayOfWeek, dayOfMonth, month, year);
    luigi.close();
    lcd.clear();
    digitalWrite(GREEN, HIGH);
    lcd.print("Luigi Santagata");
    timeOnLCD(hour, minute);
    playNote();
    delay(2000);
    digitalWrite(GREEN, LOW);
    lcd.clear();
    delay(500);
    printInstruction();
  }


 
  else //Se il UID Tag del Badge non è di Nessun Dipendente
  {
    // Crea un File Per Tutti i Badge Non Validi
    File invalid;
    invalid = SD.open("invalid.txt", FILE_WRITE);
    datetime(invalid, minute, hour, dayOfWeek, dayOfMonth, month, year);
    invalid.close();
    lcd.clear();
    digitalWrite(RED, HIGH);
    lcd.print("Non Valida");
    timeOnLCD(hour, minute);
    delay(2000);
    digitalWrite(RED, LOW);
    lcd.clear();
    delay(500);
    printInstruction();
  }
}
