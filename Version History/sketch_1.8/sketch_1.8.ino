/*
 * BigBrother 1.8 - Aldo & Eduardo Manco
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

int countEd = 0,
    countLs = 0;

int nEduardoManco = 0,
    nLuigiSantagata = 0;

//

struct genny{
  int val1;
  int val2;
};

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
}

// Scrivi su File gli Orari di Entrata e di Uscita dello Stesso Giorno Giorno sulla Stessa Riga

struct genny daytime(File file, int sameLine, int counter1, int minute, int hour, int dayOfWeek, int dayOfMonth, int month, int year){
  if(sameLine==0){
    if(file==NULL){
      datetime(file, minute, hour, dayOfWeek, dayOfMonth, month, year);
      sameLine=dayOfMonth;
      counter1=1;
    }else{
      file.println();
      datetime(file, minute, hour, dayOfWeek, dayOfMonth, month, year);
      sameLine=dayOfMonth;
      counter1=1;
    }
    }else if(sameLine==dayOfMonth){
      file.print(" ");
      if(hour<10 && hour>-1){
        file.print("0");
      }
      file.print(hour);
      file.print(":");
      if(minute<10 && minute>-1){
        file.print("0");
      }
      file.print(minute);
      if(counter1==0){
        counter1=1;
      }else if(counter1==1){
        counter1=0;
      }
    }else if(sameLine!=dayOfMonth && sameLine!=0){
      file.println();
      datetime(file, minute, hour, dayOfWeek, dayOfMonth, month, year);
      sameLine=dayOfMonth;
      counter1=1;
    }
    struct genny struttura;
    struttura.val1 = sameLine;
    struttura.val2 = counter1;//1
    return struttura;
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
  lcd.print("   ");
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
  setter(0,45,13,2,7,1,17);
  // Inizializza Schermo LCD a 2 Righe con 16 Caratteri 
  lcd.begin(16,2);
  // Sposta il Cursore sulla Parte Superiore dello Schermo LCD
  lcd.setCursor(0,0);
  // Inizializza LED Verde
  pinMode(GREEN, OUTPUT);
  // Inizializza LED Rosso
  pinMode(RED, OUTPUT);
  // Inizializza Speaker
  pinMode(speaker, OUTPUT);
  if(!SD.begin(2)) //Se il PIN CS Non è Impostato su 2
  {
    return;
  }
  // Stampa "Avvicina il Tuo Badge :)"
  printInstruction();
}



void loop() 
{
  // Se Non Viene Letto Nessun Badge
  if(!mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Se il UID Tag del Badge Non Viene Letto 
  if(!mfrc522.PICC_ReadCardSerial())
  {
    return;
  }

  // Variabili Dove Immagazzinare Data & Ora da RTC
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  // Assegna Tramite Puntatori i Valori di Data & Ora dell'RTC alle Variabili
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
    if(!SD.begin(2)) //Se il PIN CS Non è Impostato su 2
    {
      delay(60000);
    }
    // Apri File Per Scrittura
    eduardo = SD.open("mancoe.txt", FILE_WRITE);
    // Assegna alla Variabile che Indica la Giornata del Dipendente il Giorno Attuale e Scrivi nel File Data & Orari di Entrata e Uscita
    struct genny struttura2 = daytime(eduardo, nEduardoManco, countEd, minute, hour, dayOfWeek, dayOfMonth, month, year);
    nEduardoManco = struttura2.val1;
    countEd = struttura2.val2;
    // Chiudi File del Dipendente
    eduardo.close();
    // Cancella Scritta "Avvicina il Tuo Badge :)"
    lcd.clear();
    // Accendi LED Verde
    digitalWrite(GREEN, HIGH);
    // Stampa il Nome del Dipendente che ha Appena Strisciato il Badge
    lcd.print("Eduardo Manco");
    timeOnLCD(hour, minute);
    // Riproduci Suono
    playNote();
    // Leggi gli Statement che Seguono Dopo 3 Secondi, 1 della Funzione playNote() + 2 di Ritardo
    delay(2000);
    // Spegni LED Verde
    digitalWrite(GREEN, LOW);
    // Pulisci Schermo LCD
    lcd.clear();
    // Riscrivi "Avvicina il Tuo Badge :)" su Display LCD con Mezzo Secondo di Ritardo
    delay(500);
    printInstruction();
  }


  
  else if (badge.substring(1) == "C6 18 26 9E")
  {
    File luigi;
    luigi = SD.open("santagal.txt", FILE_WRITE);
    struct genny struttura2 = daytime(luigi, nLuigiSantagata, countLs, minute, hour, dayOfWeek, dayOfMonth, month, year);
    nLuigiSantagata = struttura2.val1;
    countLs = struttura2.val2;
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
