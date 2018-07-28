#include "pitches.h"
#include <TT_RFID.h>
#include <EEPROM.h>     // We are going to read and write PICC's UIDs from/to EEPROM
#include <SPI.h>        // RC522 Module uses SPI protocol
#include <MFRC522.h>  // Library for Mifare RC522 Devices
#include <Arduino.h>
#include <TT_Adafruit_NeoPixel.h>

// necessary for Adafruit Neopixels
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 44
#define NUMPIXELS 45
#define WAIT  30

constexpr uint8_t wipeB = 3;     // Button pin for WipeMode
bool programMode = false;  // initialize programming mode to false
bool successRead;    // Variable integer to keep if we have Successful Read from Reader
byte cardID[4];   // Stores scanned ID read from RFID Module
TT_RFID rfid;

TT_Adafruit_NeoPixel strip = TT_Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// for laser harp
int speakerPin = 3;
int duration = 300;

int laser1 = A0;
int laser2 = A1;
int laser3 = A2;
int laser4 = A3;
int laser5 = A4;
int laser6 = A5;
int laser7 = A6;
int laser8 = A7;
int allLasers[8] = {laser1, laser2, laser3, laser4, laser5, laser6, laser7, laser8};
int *trigLaser;

int val1;
int val2;
int val3;
int val4;
int val5;
int val6;
int val7;
int val8;

int tone1 = NOTE_C4;
int tone2 = NOTE_D4;
int tone3 = NOTE_E4;
int tone4 = NOTE_G4;
int tone5 = NOTE_A4;
int tone6 = NOTE_B4;
int tone7 = NOTE_D5;
int tone8 = NOTE_F5;
int allTones[8] = {tone1, tone2, tone3, tone4, tone5, tone6, tone7, tone8};

// for racing LEDs
int first_player;
int second_player;
const int wait = 30;
const int button1 = 40;
const int button2 = 42;
int position1 = 0;          // where player 1 starts the race
int position2 = NUMPIXELS;  // where player 2 starts the race
int color1 = RED;
int color2 = BLUE;

int IR_pin = A8;
int bigRedSwitch = 30;   // TODO: IMPLEMENT
// TODO: add serial reading from Arduino w/ LCD

int* minVal(int *allVals[]);

void setup()
{
  Serial.begin(9600);
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  pinMode(wipeB, INPUT_PULLUP);   // Enable pin's pull up resistor
  //TODO: CHECK IF RFID LIB ONLY INITIALIZES PROPER PINS FOR UNO & NOT MEGA
   rfid.initialize();
   Serial.begin(9600);  // Initialize serial communications with PC
   rfid.showReaderDetails();  // Show details of Card Reader
   rfid.toggleDeleteAllRecords(wipeB);
   bool masterDefined = rfid.isMasterDefined();
   if(!masterDefined) {
     rfid.defineMasterCard();
   }
   rfid.printInitMessage();

  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);
  pinMode(bigRedSwitch, INPUT);
  pinMode(IR_pin, INPUT);
  Serial.println("finished running setup");
}

void loop()
{

  // IR LED CODE
  int IR_val = analogRead(IR_pin);
  // someone's hand is over the sensor
  if(IR_val < 1000)
  {
    int pixelsToLight = (IR_val / 1023) * 45;
    for(int i = 0; i < pixelsToLight; i++)
    {
      strip.setPixelColor(i, TEAL);
    }
    strip.show();
    strip.begin();  // TODO: figure out how to unlight pixels
  }

  // LASER HARP CODE
  // TODO: INCLUDE INTERRUPT FOR LASER HARP
  val1 = analogRead(laser1);
  val2 = analogRead(laser2);
  val3 = analogRead(laser3);
  val4 = analogRead(laser4);
  val5 = analogRead(laser5);
  val6 = analogRead(laser6);
  val7 = analogRead(laser7);
  val8 = analogRead(laser8);
  int allVals[8] = {val1, val2, val3, val4, val5, val6, val7, val8};
  trigLaser = minVal(allVals);

  for(int i = 0; i < 8; i++)
  {
    if(*trigLaser == allLasers[i])
    {
      tone(speakerPin, allTones[i], duration);
      delay(duration);
      noTone(1);
    }
  }


  // RFID CODE
  successRead = rfid.foundID();
  rfid.getCardID(cardID);
  rfid.toggleDeleteMasterCard(wipeB);
  if(!successRead)
  {
    return;
  }
  // mode for adding and deleting RFID tags
  if (programMode)
  {
    // When in program mode check First If master card scanned again to exit program mode
    if ( rfid.isMaster(cardID) )
    {
      Serial.println(F("Master Card Scanned"));
      Serial.println(F("Exiting Program Mode"));
      Serial.println(F("-----------------------------"));
      programMode = false;
      return;
    }
    else
    {
      if ( rfid.findID(cardID) )
      { // If scanned card is known delete it
        Serial.println(F("I know this ID, removing..."));
        rfid.deleteID(cardID);
        Serial.println(F("Succesfully removed ID"));
        Serial.println("-----------------------------");
        Serial.println(F("Scan an RFID tag to ADD or REMOVE"));
      }
      else
      {                    // If scanned card is not known add it
        Serial.println(F("I do not know this ID, adding..."));
        rfid.writeID(cardID);
        Serial.println(F("Succesfully added ID"));
        Serial.println(F("-----------------------------"));
        Serial.println(F("Scan an RFID tag to ADD or REMOVE"));
      }
    }
  }

  // mode for checking IDs to see if they match existing records
  else {
    if ( rfid.isMaster(cardID)) {    // If scanned card's ID matches Master Card's ID - enter program mode
      programMode = true;
      strip.theaterChase(PURPLE, WAIT);
      Serial.println(F("Hello Master - Entered Program Mode"));
      Serial.println();
      Serial.println(F("Scan an RFID tag to ADD or REMOVE"));
      Serial.println(F("Scan Master Card again to Exit Program Mode"));
      Serial.println(F("-----------------------------"));
    }
    else {
      if ( rfid.findID(cardID) ) { // If not, see if the card is in the records
        strip.theaterChase(GREEN, WAIT);
        Serial.println(F("Welcome, You shall pass"));
      }
      else {      // If not, show that the ID was not valid
        strip.theaterChase(RED, WAIT);
        Serial.println(F("You shall not pass"));
      }
    }
  }


  // RACING LEDS CODE
  first_player = digitalRead(button1);
  second_player = digitalRead(button2);
  while(position1 < position2)
  {
    // both players pressed button simulataneously
    if(first_player == LOW && second_player == LOW)
    {
      strip.setPixelColor(position1, color1);
      ++position1;
      strip.setPixelColor(position2, color2);
      --position2;
    }
    else if(first_player == LOW)
    {
      strip.setPixelColor(position1, color1);
      ++position1;
    }
    else if(second_player == LOW)
    {
      strip.setPixelColor(position2, color2);
      --position2;
    }
  }

  // if player 1 won
  if(position1 > (NUMPIXELS / 2))
  {
    strip.theaterChase(color1, wait);
  }
  // if player 2 won
  else
  {
    strip.theaterChase(color2, wait);
  }

}

int* minVal(int allVals[])
{
  int lowestVal = 1023;
  int *lowestLaser;
  int *ptr = allVals;
  for(; ptr < ptr + 8; ++ptr)
  {
      if(*ptr < lowestVal)
      {
        lowestVal = *ptr;
        lowestLaser = ptr;
      }
  }
  return ptr;
}
