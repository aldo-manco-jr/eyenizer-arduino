/*
 * Eyenizer 2.0 - Aldo & Eduardo Manco
 * Recorder of Entrances and Exits of Employees From Building
 * Platform: Arduino/Genuino Mega or Mega 2560
 */
 
#include <SPI.h>
#include <MFRC522.h>
#include "Wire.h"
#include <SD.h>
#include "LiquidCrystal.h"
#include <Keypad.h>

#define SDA_PIN 53
#define RST_PIN 9
MFRC522 mfrc522(SDA_PIN, RST_PIN);

#define DS3231_I2C_ADDRESS 0x68

LiquidCrystal lcd(6,7,2,3,4,5);

#define GREEN 12
#define RED 11
int speaker = 10; // PIN -> 9|10|11
#define c 3830 // 261Hz 
#define d 3400 // 294Hz 
#define e 3038 // 329Hz 
#define f 2864 // 349Hz 
#define g 2550 // 392Hz 
#define a 2272 // 440Hz 
#define b 2028 // 493Hz 
#define C 1912 // 523Hz 

int nBiscegliaAlfonso,
    nBuonoMarco,
    nCalleoNicandro,
    nCambioAntonio,
    nCardarelliLorenzo,
    nColantuonoMauro,
    nCasertanoLuigi,
    nCavicchiaNicola,
    nCianchettaAlessandro,
    nColicchioAntonio,
    nDiMeoAntonio,
    nFomenikoVadim,
    nForgioneLucio,
    nGalloMauro,
    nGelfusaAntonello,
    nGuatieriFabio,
    nIannarelliFabio,
    nIannitelliAntonio,
    nManciniRenato,
    nMokkademYoussef,
    nNeriMaurizio,
    nPalombaLuigi,
    nPassarelliLorenzo,
    nPavoneNicola,
    nPetrocelliGaetano,
    nPirragliaGiuseppe,
    nRaoSalvatore,
    nRiggioneNicola,
    nRotunnoMarcello,
    nSantilliFabio,
    nScarpaleggiaBiagio,
    nSettembriniAlessio,
    nToureLamin,
    nVetteseMarco,
    nVolpeLuigi,
    nZecchiniAlessio,
    nVerrilloFederico,
    nEdyEdy;

//const byte ROWS = 4; //four rows
//const byte COLS = 3; //three columns
//char keys[ROWS][COLS] = {
//  {'#','0','*'},
//  {'9','8','7'},
//  {'6','5','4'},
//  {'3','2','1'}
//};

//byte rowPins[ROWS] = {28,27,26,25}; //connect to the row pinouts of the keypad
//byte colPins[COLS] = {24,23,22}; //connect to the column pinouts of the keypad
 
//Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Stampa sullo Schermo LCD le Istruzioni

void printInstruction(int hour, int minute){
  lcd.print(" Avvicina Badge ");
  // Sposta il Cursore sulla Parte Inferiore dello Schermo LCD
  lcd.setCursor(0, 1);
  lcd.print("     ");
  lcd.print(hour);
  lcd.print(":");
  lcd.print(minute);
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
  delay(10);
  file.print(" ");
  if(dayOfMonth<10 && dayOfMonth>-1){
    file.print("0");
    delay(10);
  }
  file.print(dayOfMonth, DEC);
  delay(10);
  file.print("/");
  delay(10);
  if(month<10 && month>-1){
    file.print("0");
    delay(10);
  }
  file.print(month, DEC);
  delay(10);
  file.print("/");
  delay(10);
  file.print(year, DEC);
  delay(10);
  file.print(" ");
  delay(10);
  if(hour<10 && hour>-1){
    file.print("0");
    delay(10);
  }
  file.print(hour, DEC);
  delay(10);
  file.print(":");
  delay(10);
  if(minute<10 && minute>-1){
    file.print("0");
    delay(10);
  }
  file.print(minute, DEC);
  delay(10);
}

// Scrivi su File gli Orari di Entrata e di Uscita dello Stesso Giorno Giorno sulla Stessa Riga

int daytime(File file, int sameLine, int minute, int hour, int dayOfWeek, int dayOfMonth, int month, int year){
  if(sameLine==0){
    if(file==NULL){
      datetime(file, minute, hour, dayOfWeek, dayOfMonth, month, year);
      sameLine=dayOfMonth;
    }else{
      file.println();
      delay(10);
      datetime(file, minute, hour, dayOfWeek, dayOfMonth, month, year);
      sameLine=dayOfMonth;
    }
    }else if(sameLine==dayOfMonth){
      file.print(" ");
      delay(10);
      if(hour<10 && hour>-1){
        file.print("0");
        delay(10);
      }
      file.print(hour);
      delay(10);
      file.print(":");
      delay(10);
      if(minute<10 && minute>-1){
        file.print("0");
        delay(10);
      }
      file.print(minute);
      delay(10);
    }else if(sameLine!=dayOfMonth && sameLine!=0){
      file.println();
      delay(10);
      datetime(file, minute, hour, dayOfWeek, dayOfMonth, month, year);
      sameLine=dayOfMonth;
    }
    return sameLine;
}

// Stampa Ora al Centro della Parte Inferiore dello Schermo LCD

void timeOnLCD(
  int dayOfMonth,
  int month,
  int hour, 
  int minute){
  // Sposta il Cursore sulla Parte Inferiore dello Schermo LCD
  lcd.setCursor(0,1);
  // Sposta il Cursore di Due Spazi
  lcd.print("  ");
  // Anteponi uno Zero se l'ora Corrente è compresa 
  if (dayOfMonth<10 && dayOfMonth>0){
    lcd.print("0");
  }
  lcd.print(dayOfMonth);
  lcd.print("/");
  if(month<10 && month>0){
  lcd.print("0");
  }
  lcd.print(month);
  if(hour<10 && hour>-1){
  lcd.print("0");
  }
  lcd.print("  ");
  // Stampa L'orario
  lcd.print(hour);
  lcd.print(":");
  // Anteponi uno Zero se il Minuto Corrente è Compreso tra 0 e 9
  if(minute<10 && minute>-1){
    lcd.print("0");
  }
  lcd.print(minute);
  lcd.print("  ");
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

// Accende il LED Verde, Mostra l'Orario di Arrivo del Dipendente, Suona 

void employee(int minute, int hour, int dayOfWeek, int dayOfMonth, int month, int year){
    // Accendi LED Verde
    digitalWrite(GREEN, HIGH);
    // Stampa l'Ora di Arrivo del Dipendente che ha Strisciato il Badge
    timeOnLCD (dayOfMonth, month, hour, minute);
    // Riproduci Suono
    playNote();
    // Leggi gli Statement che Seguono Dopo 3 Secondi, 1 della Funzione playNote() + 2 di Ritardo
    delay(2000);
    // Spegni LED Verde
    digitalWrite(GREEN, LOW);
    // Cancella Nome & Ora di Arrivo del Dipendente
    lcd.clear();
    // Mette in Attesa Per 2 Secondi Per Dare il Tempo di Scrivere i Dati sulla SD
    lcd.print("Attendere...");
    delay(1000);
    // Cancella "Attendere..."
    lcd.clear();
    // Riscrivi " Avvicina Badge /     10:23      " su Display LCD con Mezzo Secondo di Ritardo
    delay(500);
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
  setter(0,14,11,4,9,3,17);
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
  if(!SD.begin(8)) //Se il PIN CS Non è Impostato su 8
  {
    return;
  }
  lcd.setCursor(0, 1);
}



void loop() 
{
  digitalWrite(RED, LOW);
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
  lcd.print(" Avvicina Badge ");
  // Sposta il Cursore sulla Parte Inferiore dello Schermo LCD
  lcd.setCursor(0, 1);
  lcd.print("     ");
  lcd.print(hour);
  lcd.print(":");
  lcd.print(minute);
  
  // Stringa Vuota
  String badge = "";
  // Concatena UID Tag del Badge alla Stringa Vuota
  for(byte i=0; i<mfrc522.uid.size; i++) 
  {
     badge.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     badge.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  // Rendi Lettere Esadecimali della Variabile 'badge' Contenente il UID Tag Maiuscole
  badge.toUpperCase();
  
  

  //Se la Variabile Contiene il UID Tag del Badge di Colantuono Mauro
  if (badge.substring(1) == "10 9E 90 7E")
  {
     // Crea File Per Ogni Dipendente
     File mauro;
     // Apri File Per Scrittura
     mauro = SD.open("colantum.txt", FILE_WRITE);
     // Assegna alla Variabile che Indica la Giornata del Dipendente il Giorno Attuale e Scrivi nel File Data & Orari di Entrata e Uscita
     nColantuonoMauro = daytime(mauro, nColantuonoMauro, minute, hour, dayOfWeek, dayOfMonth, month, year);
     // Chiudi File del Dipendente
     mauro.close();
     // Cancella Scritta " Avvicina Badge /     10:23      "
     lcd.clear();
     // Stampa il Nome del Dipendente che ha Appena Strisciato il Badge
     lcd.print("Colantuono Mauro");
     employee(minute, hour, dayOfWeek, dayOfMonth, month, year);
  }
  
  else if (badge.substring(1) == "D0 95 8E 7E")
  {
    File luigi;
    luigi = SD.open("Casertal.txt", FILE_WRITE);
    nCasertanoLuigi = daytime(luigi, nCasertanoLuigi, minute, hour, dayOfWeek, dayOfMonth, month, year);
    luigi.close();
    lcd.clear();
    lcd.print("Casertano Luigi");
    employee(minute, hour, dayOfWeek, dayOfMonth, month, year);
  }

  else if (badge.substring(1) == "C0 CC 93 7E")
  {
    File antonio;
    antonio = SD.open("coliccha.txt", FILE_WRITE);
    nColicchioAntonio = daytime(antonio, nColicchioAntonio, minute, hour, dayOfWeek, dayOfMonth, month, year);
    antonio.close();
    lcd.clear();
    lcd.print("Colicchio Anton.");
    employee(minute, hour, dayOfWeek, dayOfMonth, month, year);
  }
  
  else if (badge.substring(1) == "F0 2A 3E 89")
  {
    File fabio;
    fabio = SD.open("iannaref.txt", FILE_WRITE);
    nIannarelliFabio = daytime(fabio, nIannarelliFabio, minute, hour, dayOfWeek, dayOfMonth, month, year);
    fabio.close();
    lcd.clear();
    lcd.print("Iannarelli Fabio");
    employee(minute, hour, dayOfWeek, dayOfMonth, month, year);
  }
  
  else if (badge.substring(1) == "60 5A 8D 7E")
  {
    File alfonso;
    alfonso = SD.open("biscegla.txt", FILE_WRITE);
    nBiscegliaAlfonso = daytime(alfonso, nBiscegliaAlfonso, minute, hour, dayOfWeek, dayOfMonth, month, year);
    alfonso.close();
    lcd.clear();
    lcd.print("Bisceglia Alfon.");
    employee(minute, hour, dayOfWeek, dayOfMonth, month, year);
  }
  
  else if (badge.substring(1) == "60 91 97 7E")
  {
    File marco;
    marco = SD.open("buonom.txt", FILE_WRITE);
    nBuonoMarco = daytime(marco, nBuonoMarco, minute, hour, dayOfWeek, dayOfMonth, month, year);
    marco.close();
    lcd.clear();
    lcd.print("Buono Marco");
    employee(minute, hour, dayOfWeek, dayOfMonth, month, year);
  }
  
  else if (badge.substring(1) == "D0 93 92 7E")
  {
    File giuseppe;
    giuseppe = SD.open("pirraglg.txt", FILE_WRITE);
    nPirragliaGiuseppe = daytime(giuseppe, nPirragliaGiuseppe, minute, hour, dayOfWeek, dayOfMonth, month, year);
    giuseppe.close();
    lcd.clear();
    lcd.print("Pirraglia Gius.");
    employee(minute, hour, dayOfWeek, dayOfMonth, month, year);
  }
  
  else if (badge.substring(1) == "B0 FF CF 7E")
  {
    File fabio;
    fabio = SD.open("guatierf.txt", FILE_WRITE);
    nGuatieriFabio = daytime(fabio, nGuatieriFabio, minute, hour, dayOfWeek, dayOfMonth, month, year);
    fabio.close();
    lcd.clear();
    lcd.print("Guatieri Fabio");
    employee(minute, hour, dayOfWeek, dayOfMonth, month, year);
  }
  
  else if (badge.substring(1) == "80 BD 99 7E")
  {
    File antonio;
    antonio = SD.open("cambioa.txt", FILE_WRITE);
    nCambioAntonio = daytime(antonio, nCambioAntonio, minute, hour, dayOfWeek, dayOfMonth, month, year);
    antonio.close();
    lcd.clear();
    lcd.print("Antonio Cambio");
    employee(minute, hour, dayOfWeek, dayOfMonth, month, year);
  }
  
  else if (badge.substring(1) == "90 63 8E 7E")
  {
    File nicola;
    nicola = SD.open("riggionn.txt", FILE_WRITE);
    nRiggioneNicola = daytime(nicola, nRiggioneNicola, minute, hour, dayOfWeek, dayOfMonth, month, year);
    nicola.close();
    lcd.clear();
    lcd.print("Riggione Nicola.");
    employee(minute, hour, dayOfWeek, dayOfMonth, month, year);
  }
  
  else if (badge.substring(1) == "60 88 95 7E")
  {
    File lorenzo;
    lorenzo = SD.open("cardarlo.txt", FILE_WRITE);
    nCardarelliLorenzo = daytime(lorenzo, nCardarelliLorenzo, minute, hour, dayOfWeek, dayOfMonth, month, year);
    lorenzo.close();
    lcd.clear();
    lcd.print("Cardarelli Lor.");
    employee(minute, hour, dayOfWeek, dayOfMonth, month, year);
  }
  
  else if (badge.substring(1) == "C0 06 93 7E")
  {
    File nicola;
    nicola = SD.open("cavicchn.txt", FILE_WRITE);
    nCavicchiaNicola = daytime(nicola, nCavicchiaNicola, minute, hour, dayOfWeek, dayOfMonth, month, year);
    nicola.close();
    lcd.clear();
    lcd.print("Cavicchia Nicola");
    employee(minute, hour, dayOfWeek, dayOfMonth, month, year);
  }
  
  else if (badge.substring(1) == "20 AE D2 7E")
  {
    File alessandro;
    alessandro = SD.open("cianchea.txt", FILE_WRITE);
    nCianchettaAlessandro = daytime(alessandro, nCianchettaAlessandro, minute, hour, dayOfWeek, dayOfMonth, month, year);
    alessandro.close();
    lcd.clear();
    lcd.print("Cianchetta Ale.");
    employee(minute, hour, dayOfWeek, dayOfMonth, month, year);
  }
  
  else if (badge.substring(1) == "B0 C5 99 7E")
  {
    File antonio;
    antonio = SD.open("dimeoa.txt", FILE_WRITE);
    nDiMeoAntonio = daytime(antonio, nDiMeoAntonio, minute, hour, dayOfWeek, dayOfMonth, month, year);
    antonio.close();
    lcd.clear();
    lcd.print("Di Meo Antonio");
    employee(minute, hour, dayOfWeek, dayOfMonth, month, year);
  }
  
  else if (badge.substring(1) == "30 A9 90 7E")
  {
    File renato;
    renato = SD.open("mancinir.txt", FILE_WRITE);
    nManciniRenato = daytime(renato, nManciniRenato, minute, hour, dayOfWeek, dayOfMonth, month, year);
    renato.close();
    lcd.clear();
    lcd.print("Mancini Renato");
    employee(minute, hour, dayOfWeek, dayOfMonth, month, year);
  }
  
  else if (badge.substring(1) == "30 13 99 7E")
  {
    File vadim;
    vadim = SD.open("fomenikv.txt", FILE_WRITE);
    nFomenikoVadim = daytime(vadim, nFomenikoVadim, minute, hour, dayOfWeek, dayOfMonth, month, year);
    vadim.close();
    lcd.clear();
    lcd.print("Fomeniko Vadim");
    employee(minute, hour, dayOfWeek, dayOfMonth, month, year);
  }
  
  else if (badge.substring(1) == "60 AF 92 7E")
  {
    File lucio;
    lucio = SD.open("forgionl.txt", FILE_WRITE);
    nForgioneLucio = daytime(lucio, nForgioneLucio, minute, hour, dayOfWeek, dayOfMonth, month, year);
    lucio.close();
    lcd.clear();
    lcd.print("Forgione Lucio");
    employee(minute, hour, dayOfWeek, dayOfMonth, month, year);
  }
  
  else if (badge.substring(1) == "50 8E 97 7E")
  {
    File antonello;
    antonello = SD.open("gelfusaa.txt", FILE_WRITE);
    nGelfusaAntonello = daytime(antonello, nGelfusaAntonello, minute, hour, dayOfWeek, dayOfMonth, month, year);
    antonello.close();
    lcd.clear(); 
    lcd.print("Gelfusa Antonel.");
    employee(minute, hour, dayOfWeek, dayOfMonth, month, year);
  }
  
  else if (badge.substring(1) == "B0 64 8E 7E")
  {
    File antonio;
    antonio = SD.open("iannitea.txt", FILE_WRITE);
    nIannitelliAntonio = daytime(antonio, nIannitelliAntonio, minute, hour, dayOfWeek, dayOfMonth, month, year);
    antonio.close();
    lcd.clear();
    lcd.print("Iannitelli Ant.");
    employee(minute, hour, dayOfWeek, dayOfMonth, month, year);
  }
  
  else if (badge.substring(1) == "20 6B 97 7E")
  {
    File youssef;
    youssef = SD.open("mokkadey.txt", FILE_WRITE);
    nMokkademYoussef = daytime(youssef, nMokkademYoussef, minute, hour, dayOfWeek, dayOfMonth, month, year);
    youssef.close();
    lcd.clear();
    lcd.print("Mokkadem Youssef");
    employee(minute, hour, dayOfWeek, dayOfMonth, month, year);
  }
  
  else if (badge.substring(1) == "00 58 99 7E")
  {
    File maurizio;
    maurizio = SD.open("nerim.txt", FILE_WRITE);
    nNeriMaurizio = daytime(maurizio, nNeriMaurizio, minute, hour, dayOfWeek, dayOfMonth, month, year);
    maurizio.close();
    lcd.clear();
    lcd.print("Neri Maurizio");
    employee(minute, hour, dayOfWeek, dayOfMonth, month, year);
  }
  
  else if (badge.substring(1) == "90 DF 94 7E")
  {
    File luigi;
    luigi = SD.open("palombal.txt", FILE_WRITE);
    nPalombaLuigi = daytime(luigi, nPalombaLuigi, minute, hour, dayOfWeek, dayOfMonth, month, year);
    luigi.close();
    lcd.clear();
    lcd.print("Palomba Luigi");
    employee(minute, hour, dayOfWeek, dayOfMonth, month, year);
  }
  
  else if (badge.substring(1) == "10 26 8F 7E")
  {
    File alessio;
    alessio = SD.open("zecchina.txt", FILE_WRITE);
    nZecchiniAlessio = daytime(alessio, nZecchiniAlessio, minute, hour, dayOfWeek, dayOfMonth, month, year);
    alessio.close();
    lcd.clear();
    lcd.print("Zecchini Alessio");
    employee(minute, hour, dayOfWeek, dayOfMonth, month, year);
  }
  
  else if (badge.substring(1) == "90 50 8E 7E")
  {
    File salvatore;
    salvatore = SD.open("raos.txt", FILE_WRITE);
    nRaoSalvatore = daytime(salvatore, nRaoSalvatore, minute, hour, dayOfWeek, dayOfMonth, month, year);
    salvatore.close();
    lcd.clear();
    lcd.print("Rao Salvatore");
    employee(minute, hour, dayOfWeek, dayOfMonth, month, year);
  }
  
  else if (badge.substring(1) == "80 B3 99 7E")
  {
    File marcello;
    marcello = SD.open("marcellr.txt", FILE_WRITE);
    nRotunnoMarcello = daytime(marcello, nRotunnoMarcello, minute, hour, dayOfWeek, dayOfMonth, month, year);
    marcello.close();
    lcd.clear();
    lcd.print("Rotunno Marcello");
    employee(minute, hour, dayOfWeek, dayOfMonth, month, year);
  }
  
  
  else if (badge.substring(1) == "D0 98 8D 7E")
  {
    File biagio;
    biagio = SD.open("scarpalb.txt", FILE_WRITE);
    nScarpaleggiaBiagio = daytime(biagio, nScarpaleggiaBiagio, minute, hour, dayOfWeek, dayOfMonth, month, year);
    biagio.close();
    lcd.clear();
    lcd.print("Scarpaleggia B.");
    employee(minute, hour, dayOfWeek, dayOfMonth, month, year);
  }
  
  else if (badge.substring(1) == "C0 6C 8D 7E")
  {
    File alessio;
    alessio = SD.open("settemba.txt", FILE_WRITE);
    nSettembriniAlessio = daytime(alessio, nSettembriniAlessio, minute, hour, dayOfWeek, dayOfMonth, month, year);
    alessio.close();
    lcd.clear();
    lcd.print("Settembrini Ale.");
    employee(minute, hour, dayOfWeek, dayOfMonth, month, year);
  }
  
    
  else if (badge.substring(1) == "00 51 93 7E")
  {
    File lamin;
    lamin = SD.open("tourel.txt", FILE_WRITE);
    nToureLamin = daytime(lamin, nToureLamin, minute, hour, dayOfWeek, dayOfMonth, month, year);
    lamin.close();
    lcd.clear();
    lcd.print("Toure Lamin");
    employee(minute, hour, dayOfWeek, dayOfMonth, month, year);
  }
  
  else if (badge.substring(1) == "70 8E 97 7E")
  {
    File marco;
    marco = SD.open("vettesem.txt", FILE_WRITE);
    nVetteseMarco = daytime(marco, nVetteseMarco, minute, hour, dayOfWeek, dayOfMonth, month, year);
    marco.close();
    lcd.clear();
    lcd.print("Vettese Marco");
    employee(minute, hour, dayOfWeek, dayOfMonth, month, year);
  }
  
  else if (badge.substring(1) == "C0 6F 8D 7E")
  {
    File luigi;
    luigi = SD.open("volpel.txt", FILE_WRITE);
    nVolpeLuigi = daytime(luigi, nVolpeLuigi, minute, hour, dayOfWeek, dayOfMonth, month, year);
    luigi.close();
    lcd.clear();
    lcd.print("Volpe Luigi");
    employee(minute, hour, dayOfWeek, dayOfMonth, month, year);
  }

  else if (badge.substring(1) == "30 AD 92 7E")
  {
    File federico;
    federico = SD.open("verrillf.txt", FILE_WRITE);
    nVerrilloFederico = daytime(federico, nVerrilloFederico, minute, hour, dayOfWeek, dayOfMonth, month, year);
    federico.close();
    lcd.clear();
    lcd.print("Verrillo Feder.");
    employee(minute, hour, dayOfWeek, dayOfMonth, month, year);
  }
  
  else if (badge.substring(1) == "80 E5 9B 7E")
  {
    File mauro;
    mauro = SD.open("gallom.txt", FILE_WRITE);
    nGalloMauro = daytime(mauro, nGalloMauro, minute, hour, dayOfWeek, dayOfMonth, month, year);
    mauro.close();
    lcd.clear();
    lcd.print("Gallo Mauro");
    employee(minute, hour, dayOfWeek, dayOfMonth, month, year);
  }
  
  else if (badge.substring(1) == "D0 E9 8F 7E")
  {
    File lorenzo;
    lorenzo = SD.open("passarlo.txt", FILE_WRITE);
    nPassarelliLorenzo = daytime(lorenzo, nPassarelliLorenzo, minute, hour, dayOfWeek, dayOfMonth, month, year);
    lorenzo.close();
    lcd.clear();
    lcd.print("Passarelli Lor.");
    employee(minute, hour, dayOfWeek, dayOfMonth, month, year);
  }

  else if (badge.substring(1) == "50 E0 93 7E")
  {
    File nicola;
    nicola = SD.open("pavonen.txt", FILE_WRITE);
    nPavoneNicola = daytime(nicola, nPavoneNicola, minute, hour, dayOfWeek, dayOfMonth, month, year);
    nicola.close();
    lcd.clear();
    lcd.print("Pavone Nicola");
    employee(minute, hour, dayOfWeek, dayOfMonth, month, year);
  }

  else if (badge.substring(1) == "D0 E8 9B 7E")
  {
    File gaetano;
    gaetano = SD.open("petroceg.txt", FILE_WRITE);
    nPetrocelliGaetano = daytime(gaetano, nPetrocelliGaetano, minute, hour, dayOfWeek, dayOfMonth, month, year);
    gaetano.close();
    lcd.clear();
    lcd.print("Petrocelli Gaet.");
    employee(minute, hour, dayOfWeek, dayOfMonth, month, year);
  }

  else if (badge.substring(1) == "50 C6 CB 7E")
  {
    File fabio;
    fabio = SD.open("santillf.txt", FILE_WRITE);
    nSantilliFabio = daytime(fabio, nSantilliFabio, minute, hour, dayOfWeek, dayOfMonth, month, year);
    fabio.close();
    lcd.clear();
    lcd.print("Santilli Fabio");
    employee(minute, hour, dayOfWeek, dayOfMonth, month, year);
  }
  
  else if (badge.substring(1) == "E0 B2 8D 7E")
  {
    File edy;
    edy = SD.open("edy.txt", FILE_WRITE);
    nEdyEdy = daytime(edy, nEdyEdy, minute, hour, dayOfWeek, dayOfMonth, month, year);
    edy.close();
    lcd.clear();
    lcd.print("Eduardo Manco");
    employee(minute, hour, dayOfWeek, dayOfMonth, month, year);
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
    (dayOfMonth, month, hour, minute);
    delay(2000);
    digitalWrite(RED, LOW);
    lcd.clear();
    delay(500);
    printInstruction(hour, minute);
  }
}
