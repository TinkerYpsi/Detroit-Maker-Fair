#include "Adafruit_APDS9960.h"
Adafruit_APDS9960 apds;

// physical button connections
int up_button = 10;
int click_button = 9;
int down_button = 8;

// send as output to Uno w/ LCD
int up_pin;
int down_pin;
int click_pin;

void setup()
{
  pinMode(up_pin, OUTPUT);
  pinMode(down_pin, OUTPUT);
  pinMode(click_pin, OUTPUT);
  pinMode(up_button, INPUT_PULLUP);
  pinMode(click_button, INPUT_PULLUP);
  pinMode(down_button, INPUT_PULLUP);
  Serial.begin(9600);

  if(!apds.begin())
  {
    Serial.println("failed to initialize device! Please check your wiring.");
  }
  else Serial.println("Device initialized!");

  //gesture mode will be entered once proximity mode senses something close
  apds.enableProximity(true);
  apds.enableGesture(true);
}

void loop()
{
  int up_status = digitalRead(up_button);
  int click_status = digitalRead(click_button);
  int down_status = digitalRead(down_button);
  if(up_status == LOW)
  {
    digitalWrite(up_pin, HIGH);
  }
  else if(click_status == LOW)
  {
    digitalWrite(click_pin, HIGH);
  }
  else if(down_status == LOW)
  {
    digitalWrite(down_pin, HIGH);
  }

  //read a gesture from the device
    uint8_t gesture = apds.readGesture();
    if(gesture == APDS9960_UP)
    {
      Serial.println("SELECT");
      digitalWrite(click_pin, HIGH);
    }
    if(gesture == APDS9960_LEFT)
    {
      Serial.println("UP");
      digitalWrite(up_pin, HIGH);
    }
    if(gesture == APDS9960_RIGHT)
    {
      Serial.println("DOWN");
      digitalWrite(down_pin, HIGH);
    }

    digitalWrite(click_pin, LOW);
    digitalWrite(up_pin, LOW);
    digitalWrite(down_pin, LOW);
}
