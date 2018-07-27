#include <LiquidCrystal.h>
#include <cactus_io_AM2302.h>

#define AM2302_PIN 2     // pin of humidity sensor data line

// select the pins used on the LCD panel
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);      //default pins for the Arduino Uno
AM2302 dht(AM2302_PIN);

// define some values used by the panel and buttons
int lcd_key     = 0;
int adc_key_in  = 0;
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

const int top_button = A0;
const int mid_button = A1;
const int bottom_button = A2;

// read the onboard buttons
int read_onboard_LCD_buttons()
{
 adc_key_in = analogRead(0);      // read the value from the sensor
 // my buttons when read are centered at these values: 0, 144, 329, 504, 741
 // we add approx 50 to those values and check to see if we are close

 if (adc_key_in < 50)   return btnRIGHT;
 if (adc_key_in < 195)  return btnUP;
 if (adc_key_in < 380)  return btnDOWN;
 if (adc_key_in < 555)  return btnLEFT;
 if (adc_key_in < 790)  return btnSELECT;



 return btnNONE;  // when all others fail, return this...
}

// read external read_onboard_LCD_buttons
int read_external_LCD_buttons()
{
  int up = digitalRead(top_button);
  int click = digitalRead(mid_button);
  int down = digitalRead(bottom_button);

  // up was pressed
  if(click == LOW)
  {
    return btnSELECT;
  }
  else if(down == LOW)
  {
    return btnDOWN;
  }
  else if(up == LOW)
  {
    return btnUP;
  }
  else
  {
    return btnNONE;
  }
}

void setup()
{
 lcd.begin(16, 2);              // start the library
 dht.begin();                   // start the humidity sensor
 lcd.setCursor(0,0);
 pinMode(top_button, INPUT_PULLUP);
 pinMode(mid_button, INPUT_PULLUP);
 pinMode(bottom_button, INPUT_PULLUP);
 lcd.print("Push the buttons"); // print a simple message
 Serial.begin(9600);
 Serial.println("RH\t\tTemp (F)\n");
}

void loop()
{

  // HUMIDITY SENSOR CODE
  dht.readHumidity();
  dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(dht.humidity) || isnan(dht.temperature_C)) {
    Serial.println("DHT sensor read failure!");
    return;
  }
  lcd.setCursor(0,0);   // set cursor to 1st column, 1st row
  lcd.print("Humidity: ");
  lcd.print(dht.humidity); lcd.print("%");
  Serial.print(dht.humidity); Serial.print(" %\t\t");
  lcd.setCursor(0,1);   // set cursor to 1st column, 2nd row
  lcd.print("Temp: ");
  lcd.print(dht.temperature_F); lcd.print("F");
  Serial.print(dht.temperature_F); Serial.print(" *F\n");

  // Wait a few seconds between measurements. The AM2302 should not be read at a higher frequency of
  // about once every 2 seconds. So we add a 3 second delay to cover this.
  delay(3000);


  // BUTTON READING CODE
 lcd.setCursor(0,1);            // move to the begining of the second line
 lcd_key = read_external_LCD_buttons();  // read the external buttons

 switch (lcd_key)               // depending on which button was pushed, we perform an action
 {
   case btnUP:
     {
     lcd.print("UP    ");
     break;
     }
   case btnDOWN:
     {
     lcd.print("DOWN  ");
     break;
     }
   case btnSELECT:
     {
     lcd.print("SELECT");
     break;
     }
     case btnNONE:
     {
     lcd.print("NONE  ");
     break;
     }
 }
}
