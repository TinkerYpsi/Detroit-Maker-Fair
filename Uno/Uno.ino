#include "Adafruit_APDS9960.h"
Adafruit_APDS9960 apds;

// physical button connections
int up_button = 12;
int click_button = 9;
int down_button = 8;

// output pins to LCD
int up_output = 6;
int click_output = 7;
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

  if(up_status == LOW && prev_up_status == HIGH)
  {
    digitalWrite(up_output, HIGH);
    Serial.println("UP");
  }
  if(click_status == LOW && prev_click_status == HIGH)
  {
    digitalWrite(click_output, HIGH);
    Serial.println("SELECT");
  }
  if(down_status == LOW && prev_down_status == HIGH)
  {
    digitalWrite(down_output, HIGH);
    Serial.println("DOWN");
  }

  //read a gesture from the device
  uint8_t gesture = apds.readGesture();
  if(gesture == APDS9960_UP || gesture == APDS9960_DOWN)
  {
    Serial.println("SELECT");
    digitalWrite(click_output, HIGH);
  }
  if(gesture == APDS9960_LEFT)
  {
    Serial.println("UP");
    digitalWrite(up_output, HIGH);
  }
  if(gesture == APDS9960_RIGHT)
  {
    Serial.println("DOWN");
    digitalWrite(down_output, HIGH);
  }

  digitalWrite(up_output, LOW);
  digitalWrite(click_output, LOW);
  digitalWrite(down_output, LOW);

  prev_up_status = up_status;
  prev_click_status = click_status;
  prev_down_status = down_status;

  delay(10);
}
