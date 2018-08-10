#include <TimerOne.h>
#include "pitches.h"
#include <EEPROM.h>     // We are going to read and write PICC's UIDs from/to EEPROM
#include <SPI.h>        // RC522 Module uses SPI protocol
#include <Arduino.h>
#include <TT_Adafruit_NeoPixel.h>
#include <MFRC522.h>
#include <Wire.h>
#include <TT_TouchKeypadTTP229.h>

// necessary for Adafruit Neopixels
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 44
#define NUMPIXELS 45

constexpr uint8_t RST_PIN = 5;     // Configurable, see typical pin layout above
constexpr uint8_t SS_PIN = 53;     // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance

constexpr uint8_t wipeB = 3;     // Button pin for WipeMode
bool programMode = false;  // initialize programming mode to false
bool successRead;    // Variable integer to keep if we have Successful Read from Reader

TT_Adafruit_NeoPixel strip = TT_Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
TT_TouchKeypadTTP229 touchpad;

// for laser harp
const int LASER_THRESHOLD = 130;
const int speakerPin = 3;

const int LASER_COUNT = 8;
const int LASER[LASER_COUNT] = {A0, A1, A2, A3, A4, A5, A6, A7};

int laser_val[8] = {0};

const int tone1 = NOTE_C4;
const int tone2 = NOTE_D4;
const int tone3 = NOTE_E4;
const int tone4 = NOTE_F4;
const int tone5 = NOTE_G4;
const int tone6 = NOTE_A4;
const int tone7 = NOTE_B4;
const int tone8 = NOTE_C5;
const int allTones[8] = {tone1, tone2, tone3, tone4, tone5, tone6, tone7, tone8};

// for racing LEDs
const int WAIT_TIME = 30;
const int BUTTON1 = 40;
const int BUTTON2 = 41;

const int JOY_BUTTON = 47;
const int JOY_X = A14;
const int JOY_Y = A15;


const int IR_pin = A8;
const int bigRedSwitch = 2;

void setup()
{
  Serial.begin(9600);

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  pinMode(wipeB, INPUT_PULLUP);   // Enable pin's pull up resistor
//  //TODO: CHECK IF RFID LIB ONLY INITIALIZES PROPER PINS FOR UNO & NOT MEGA
//   rfid.initialize();
//   rfid.showReaderDetails();  // Show details of Card Reader
//   rfid.toggleDeleteAllRecords(wipeB);
//   bool masterDefined = rfid.isMasterDefined();
//   if(!masterDefined) {
//     rfid.defineMasterCard();
//   }
//   rfid.printInitMessage();

  SPI.begin();                                                  // Init SPI bus
  mfrc522.PCD_Init();                                              // Init MFRC522 card


  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);
  pinMode(bigRedSwitch, INPUT_PULLUP);
  pinMode(IR_pin, INPUT);

  // Timer1.initialize(10000);         // initialize timer1, and set a 1/2 second period
  // Timer1.attachInterrupt(runRedSwitch);  // attaches callback() as a timer overflow interrupt

  Serial.println("finished running setup");
  attachInterrupt(digitalPinToInterrupt(bigRedSwitch), runRedSwitch, RISING);
}

typedef enum _mode{
  RACE,
  HARP,
  DISTANCE,
  JOYSTICK,
  RFID,
  TOUCHPAD,
  SWITCH,
  TEMP,
}MODE;

MODE mode = RFID;

void loop()
{
  if(Serial.available())
  {
    delay(15);
    while(Serial.available())
    {
      char input = Serial.read();
      if(input == 'r'){
        mode = RACE;
        Serial.println("Race");
        clearPixels();
        strip.show();
      }
      else if(input == 'h'){
        mode = HARP;
        Serial.println("Harp");
        clearPixels();
        strip.show();
      }
      else if(input == 'd'){
        mode = DISTANCE;
        Serial.println("Distance Sensor");
        clearPixels();
        strip.show();
      }
      else if(input == 'j'){
        mode = JOYSTICK;
        Serial.println("Joystick");
        clearPixels();
        strip.show();
      }
      else if(input == 'i'){
        mode = RFID;
        Serial.println("RFID");
        clearPixels();
        strip.show();
      }
      else if(input == 's'){
        mode = SWITCH;
        Serial.println("Big Red Switch");
        clearPixels();
        strip.show();
      }
    }
  }
  switch(mode)
  {
    case RACE:
    {
      runRace();
      break;
    }
    case HARP:
    {
      runHarp();
      break;
    }
    case DISTANCE:
    {
      runDistanceSensor();
      break;
    }
    case JOYSTICK:
    {
      runJoystick();
      break;
    }
    case RFID:
    {
      runRFID();
      break;
    }
    case SWITCH:
    {
      runRedSwitch();
      break;
    }
    default:
    {
      break;
    }
  }
}

void runRedSwitch(){
  while(!Serial.available() && digitalRead(bigRedSwitch) == HIGH)
  {
    int delay = 10;
    clearPixels();
    strip.theaterChase(RED, delay);
  }
}

void runJoystick(){
  while(!Serial.available())
  {
    int x_val = 0;
    int y_val = 0;
    bool button_val = digitalRead(JOY_BUTTON);
    const int SAMPLES = 20;

    for(int i = 0; i < SAMPLES; i++)
    {
      x_val += analogRead(JOY_X);
      y_val += analogRead(JOY_Y);
    }
    x_val /= SAMPLES;
    y_val /= SAMPLES;


    int pixel = map(x_val, 0, 800, 0, 45);
    Serial.print("pixel: ");
    Serial.println(pixel);
    long color = Wheel(map(y_val, 0, 800, 0, 255));
    clearPixels();
    strip.setPixelColor(pixel, color);
    strip.show();

    char output[255];
    sprintf(output, "X: %i  Y: %i  B: %i", x_val, y_val, button_val);
    Serial.println(output);
    delay(100);
  }
}

void runDistanceSensor(){
// IR LED CODE
  while(!Serial.available())
  {
    const int SAMPLES = 20;
    int ir_val = 0;
    for(int i = 0; i < SAMPLES; i++){
      ir_val += analogRead(IR_pin);
    }
    ir_val /= SAMPLES;

    Serial.print("ir_val: ");
    Serial.println(ir_val);
    int pixels_to_light = map(ir_val, 540, 170, 0, 45);

    for(int i = 0; i < NUMPIXELS; i++)
    {
      if(i < pixels_to_light){
        strip.setPixelColor(i, strip.Color(15, 35, 25));
      }
      else{
        strip.setPixelColor(i, strip.Color(0, 0, 0));
      }
    }
    strip.show();
  }
}

void runHarp(){
  // LASER HARP CODE
  // TODO: INCLUDE INTERRUPT FOR LASER HARP
  for(int i = 0; i < LASER_COUNT; i++){
    int this_val = analogRead(LASER[i]);
    laser_val[i] = this_val;
//    Serial.print("Laser ");
//    Serial.print(i);
//    Serial.print(": ");
//    Serial.println(this_val);
  }
  int trig_laser = minIndex(laser_val);
//  Serial.print("Active laser: ");
//  Serial.println(trig_laser);
//  Serial.println("");

  if(trig_laser >= 0){
    tone(speakerPin, allTones[trig_laser], 50);
  }
  else
  {
    noTone(1);
  }
}

void dump_byte_array(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}

unsigned long getUIDVal(){
  unsigned long ret = 0;
  for(int i = 0; i < 4; i++){
    ret |= mfrc522.uid.uidByte[i];
    ret = ret << 8;
  }
  return ret;
}

void runRFID(){
  while(!Serial.available())
  {
    const int TAG_COUNT = 3;
    unsigned long id[TAG_COUNT] = {0};

    for(int i = 0; i < TAG_COUNT; i++)
    {
      Serial.print("Present key ");
      Serial.println(i);
      while(1)
      {
        if(Serial.available())
        {
          return;
        }
        if(mfrc522.PICC_IsNewCardPresent())
        {
          if(mfrc522.PICC_ReadCardSerial()){
            unsigned long thisUID = getUIDVal();
            bool is_new = true;
            for(int j = 0; j < TAG_COUNT; j++){
              if(id[j] == thisUID)
              {
                is_new = false;
              }
            }
            if(is_new)
            {
              id[i] = thisUID;
              Serial.print("id ");
              Serial.print(i);
              Serial.print(": ");
              Serial.println(id[i]);
              int pixels_per_inc = strip.numPixels() / TAG_COUNT;
              for(int j = 0; j < pixels_per_inc; j++)
              {
                strip.setPixelColor((j + i * pixels_per_inc), strip.Color(0, 0, 40));
                strip.show();
                delay(15);
                if(i == TAG_COUNT - 1 && j == pixels_per_inc - 1 && ((j + i * pixels_per_inc) < strip.numPixels() - 1))
                {
                  strip.setPixelColor(strip.numPixels() - 1, strip.Color(0, 0, 40));
                  strip.show();
                }
              }
              Serial.println("SLDJFLJWEF:EJ");
              break;
            }
          }
        }
      }
    }

    while(!Serial.available())
    {
      unsigned long lastUID = 0;
      int command = -1;
      // Look for new cards
      if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial())
      {
        long thisUID = getUIDVal();
        if(lastUID != thisUID)
        {
          for(int i = 0; i < TAG_COUNT; i++)
          {
            if(id[i] == thisUID)
            {
              command = i;
            }
          }
        }
      }

      switch(command)
      {
        case(0):
        {
          knightRider(15);
          colorWipe(0, 25);
          break;
        }
        case(1):
        {
          theaterChase(Wheel(random(0, 255)), 25);
          colorWipe(0, 25);
          break;
        }
        case(2):
        {
          rainbowCycle(5);
          colorWipe(0, 25);
          break;
        }
        case(3):
        {
          colorWipe(Wheel(random(0, 255)), 25);
          colorWipe(0, 25);
          break;
        }
      }
    }
  }
}

void runRace(){
  // RACING LEDS CODE
  int position1 = 0;          // where player 1 starts the race
  int position2 = NUMPIXELS;  // where player 2 starts the race
  bool last_p1 = HIGH;
  bool last_p2 = HIGH;
  long color1 = strip.Color(40, 0, 0);
  long color2 = strip.Color(0, 0, 40);

  while(position1 < NUMPIXELS /2 && position2 > NUMPIXELS /2)
  {
    bool p1 = digitalRead(BUTTON1);
    bool p2 = digitalRead(BUTTON2);
    // both players pressed button simulataneously
    if((!p1 && last_p1) && (!p2 && last_p2))
    {
      strip.setPixelColor(position1, color1);
      ++position1;
      strip.setPixelColor(position2, color2);
      --position2;
    }
    else if((!p1 && last_p1))
    {
      strip.setPixelColor(position1, color1);
      ++position1;
    }
    else if((!p2 && last_p2))
    {
      strip.setPixelColor(position2, color2);
      --position2;
    }
    strip.show();
    last_p1 = p1;
    last_p2 = p2;
    if(Serial.available())
    {
      clearPixels();
      strip.show();
      break;
    }
  }

  // if player 1 won
  if(position1 > (NUMPIXELS - position2))
  {
    theaterChase(color1, 50);
  }
  // if player 2 won
  else
  {
    theaterChase(color2, 50);
  }
}

void clearPixels(){
  for(int i = 0; i < strip.numPixels(); i++){
    strip.setPixelColor(i, strip.Color(0, 0, 0));
  }
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

void knightRider(uint8_t wait){
  const int ON_COUNT = 9;
  bool dir = true;
  for(int k = 0; k < 4; k++)
  {
    for(int i = 0; i < strip.numPixels() - ON_COUNT + 1; i++)
    {
      clearPixels();
      int brightness = 0;
      for(int j = 0; j < ON_COUNT; j++)
      {
        if(j == 0) brightness = 5;
        if(j == 1) brightness = 25;
        if(j == 2) brightness = 50;
        if(j == 3) brightness = 100;
        if(j == 4) brightness = 250;
        if(j == 5) brightness = 100;
        if(j == 6) brightness = 50;
        if(j == 7) brightness = 25;
        if(j == 8) brightness = 5;
        Serial.println(brightness);
        int pixel = dir ? i + j : (strip.numPixels()) - 1 - (i + j);
        strip.setPixelColor(pixel, strip.Color(brightness, 0, 0));
      }
      strip.show();
      delay(wait);
      Serial.println("");
    }
    dir = !dir;
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}


// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) { // 1 cycle of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

int minIndex(int* values)
{
  int lowest_val = 1023;
  int lowest_index = 0;
  for(int i = 0; i < LASER_COUNT; i++){
    if(values[i] < lowest_val){
      lowest_val = values[i];
      lowest_index = i;
    }
  }
  if(lowest_val > LASER_THRESHOLD){
    lowest_index = -1;
  }
  return lowest_index;
}

void checkInputs() {}
