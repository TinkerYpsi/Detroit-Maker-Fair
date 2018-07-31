#include "Adafruit_APDS9960.h"
Adafruit_APDS9960 apds;

// physical button connections
int up_button = 10;
int click_button = 9;
int down_button = 8;

// output pins to LCD
int up_output = 6;
int click_output = 7;
int down_output = 5;

void setup()
{
  pinMode(up_button, INPUT_PULLUP);
  pinMode(click_button, INPUT_PULLUP);
  pinMode(down_button, INPUT_PULLUP);
  pinMode(up_output, OUTPUT);
  pinMode(click_output, OUTPUT);
  pinMode(down_output, OUTPUT);
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
    digitalWrite(up_output, HIGH);
    delay(5);
  }
  if(click_status == LOW)
  {
    digitalWrite(click_output, HIGH);
    delay(5);
  }
  if(down_status == LOW)
  {
    digitalWrite(down_output, HIGH);
    delay(5);
  }

  //read a gesture from the device
  // uint8_t gesture = apds.readGesture();
  // if(gesture == APDS9960_UP)
  // {
  //   Serial.println("SELECT");
  //   digitalWrite(click_pin, HIGH);
  // }
  // if(gesture == APDS9960_LEFT)
  // {
  //   Serial.println("UP");
  //   digitalWrite(up_pin, HIGH);
  // }
  // if(gesture == APDS9960_RIGHT)
  // {
  //   Serial.println("DOWN");
  //   digitalWrite(down_pin, HIGH);
  // }

  digitalWrite(up_output, LOW);
  digitalWrite(click_output, LOW);
  digitalWrite(down_output, LOW);
}
