#include "Adafruit_APDS9960.h"
#include <Wire.h>
#include <TT_TouchKeypadTTP229.h>


Adafruit_APDS9960 apds;
TT_TouchKeypadTTP229 touchpad;

// physical button connections
int up_button = 12;
int click_button = 9;
int down_button = 8;

// output pins to LCD
int up_output = 6;
int interruptPin = 7;
int down_output = 5;

int prev_up_status;
int prev_click_status;
int prev_down_status;


void setup()
{
  pinMode(up_button, INPUT_PULLUP);
  pinMode(click_button, INPUT_PULLUP);
  pinMode(down_button, INPUT_PULLUP);
  pinMode(up_output, OUTPUT);
  pinMode(interruptPin, OUTPUT);
  pinMode(down_output, OUTPUT);
  Serial.begin(115200);

  if(!apds.begin())
  {
    Serial.println("failed to initialize device! Please check your wiring.");
  }
  else Serial.println("Device initialized!");

  // gesture mode will be entered once proximity mode senses something close
  apds.enableProximity(true);
  apds.enableGesture(true);
}

void loop()
{
  digitalWrite(up_output, HIGH);
  digitalWrite(down_output, HIGH);
  digitalWrite(interruptPin, HIGH);

  int up_status = digitalRead(up_button);
  int click_status = digitalRead(click_button);
  int down_status = digitalRead(down_button);

  if(up_status == LOW && prev_up_status == HIGH)
  {
    digitalWrite(up_output, LOW);
    digitalWrite(down_output, HIGH);
    digitalWrite(interruptPin, LOW);
    Serial.println("UP");
  }
  if(click_status == LOW && prev_click_status == HIGH)
  {
    digitalWrite(up_output, LOW);
    digitalWrite(down_output, LOW);
    digitalWrite(interruptPin, LOW);
    Serial.println("SELECT");
  }
  if(down_status == LOW && prev_down_status == HIGH)
  {
    digitalWrite(down_output, LOW);
    digitalWrite(up_output, HIGH);
    digitalWrite(interruptPin, LOW);
    Serial.println("DOWN");
  }

  // read a gesture from the device
  uint8_t gesture = apds.readGesture();
  if(gesture == APDS9960_UP || gesture == APDS9960_DOWN)
  {
    digitalWrite(up_output, LOW);
    digitalWrite(down_output, LOW);
    digitalWrite(interruptPin, LOW);
    Serial.println("SELECT");
    delay(10);
  }
  if(gesture == APDS9960_LEFT)
  {
    digitalWrite(up_output, LOW);
    digitalWrite(down_output, HIGH);
    digitalWrite(interruptPin, LOW);
    Serial.println("UP");
    delay(10);
  }
  if(gesture == APDS9960_RIGHT)
  {
    digitalWrite(down_output, LOW);
    digitalWrite(up_output, HIGH);
    digitalWrite(interruptPin, LOW);
    Serial.println("DOWN");
    delay(10);
  }

  // read the touchpad keys
  if(touchpad.hasInput())
  {
    byte pass1;   // first half of the code entered
    byte pass2;   // second half of the code entered
    touchpad.getKeys(&pass1, &pass2);
    // Serial.print("password: ");
    // Serial.print(pass1);
    // Serial.print(", ");
    // Serial.println(pass2);
    if(pass2 == 0b01000000)         // code is 10
    {
      digitalWrite(up_output, LOW);
      digitalWrite(down_output, HIGH);
      digitalWrite(interruptPin, LOW);
      Serial.println("UP");
      delay(10);
    }
    else if(pass2 == 0b00001000)   // code is 13
    {
      digitalWrite(down_output, LOW);
      digitalWrite(up_output, HIGH);
      digitalWrite(interruptPin, LOW);
      Serial.println("DOWN");
      delay(10);
    }
    else if(pass1 == 0b10000000)   // code is 1
    {
      digitalWrite(up_output, LOW);
      digitalWrite(down_output, LOW);
      digitalWrite(interruptPin, LOW);
      Serial.println("SELECT");
      delay(10);
    }
  }

  prev_up_status = up_status;
  prev_click_status = click_status;
  prev_down_status = down_status;

}
