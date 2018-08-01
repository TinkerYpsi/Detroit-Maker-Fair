#include <TT_LiquidCrystal.h>
#include <cactus_io_AM2302.h>

// define some values used by the panel and buttons
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

int humidity_pin = 2;
int down_input = 11;
int interruptPin = 3;
int up_input = 12;
int mega_output = 13;
int i = 0;
String selected_option;
String prev_selected_option;
bool humidity_mode = false;
int btnClicked = btnNONE;

String options[7] = {"Race", "Laser Harp", "RFID", "Temp/Humidity", "Distance sensor", "Touchpad", "Big Red Switch"};

int read_onboard_LCD_buttons();
void switchMode(String selected_option);
void read_external_LCD_buttons();

TT_LiquidCrystal lcd(8, 9, 4, 5, 6, 7);      //default pins for the Arduino Uno
AM2302 dht(humidity_pin);


void setup()
{
  Serial.begin(9600);
  pinMode(humidity_pin, INPUT_PULLUP);
  pinMode(down_input, INPUT);
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), read_external_LCD_buttons, FALLING);
  pinMode(up_input, INPUT);
  pinMode(mega_output, OUTPUT);
  lcd.begin(16,2);
  lcd.setCursor(0,0);
  lcd.print(options[0]);
  Serial.println(options[0]);
  lcd.cursor();
}

void loop()
{
  switch(btnClicked)
  {
    case btnUP:
    {
      humidity_mode = false;
      lcd.clear();
      // if i is less than size of options array
      if(i < sizeof(options)/sizeof(String))
      {
        lcd.print(options[i + 1]);
        Serial.println(options[i + 1]);
        i++;
      }
      else
      {
        i = 0;
        lcd.print(options[i]);
        Serial.println(options[i]);
      }
      lcd.cursor();
      btnClicked = btnNONE;
      break;
    }
    case btnDOWN:
    {
      humidity_mode = false;
      lcd.clear();
      if(i > 0)
      {
        lcd.print(options[i - 1]);
        Serial.println(options[i - 1]);
        i--;
      }
      else
      {
        // i is equal to size of options array
        i = sizeof(options) / sizeof(String);
        lcd.print(options[i]);
      }
      lcd.cursor();
      btnClicked = btnNONE;
      break;
    }
    case btnSELECT:
    {
      humidity_mode = false;
      lcd.noCursor();
      selected_option = options[i];
      btnClicked = btnNONE;
      break;
    }
    case btnNONE:
    {
      // do nothing
      break;
    }
  }

  if(prev_selected_option != selected_option)
  {
    switchMode(selected_option);
    prev_selected_option = selected_option;
  }

  static long last_check = millis();
  if(humidity_mode && ((millis() - last_check) > 3000))
  {
    last_check = millis();
    Serial.print("last check: ");
    Serial.println(last_check);
    Serial.print("time: ");
    Serial.println(millis());
    lcd.clear();
    dht.readHumidity();
    dht.readTemperature();

    // Check if any reads failed and exit early (to try again).
    if (isnan(dht.humidity) || isnan(dht.temperature_C)) {
      Serial.println("DHT sensor read failure!");
      return;
    }

    if(btnClicked != btnNONE)
    {
      humidity_mode = false;
    }

    lcd.setCursor(0,0);   // set cursor to 1st column, 1st row
    lcd.print("Humidity: ");
    lcd.print(dht.humidity); lcd.print("%");
    Serial.print(dht.humidity); Serial.print(" %\t\t");
    lcd.setCursor(0,1);   // set cursor to 1st column, 2nd row
    lcd.print("Temp: ");
    lcd.print(dht.temperature_F); lcd.print("F");
    Serial.print(dht.temperature_F); Serial.print(" *F\n");
  }

}

void switchMode(String selected_option)
{
  // Race
  if(selected_option == options[0])
  {
    Serial.write("r");
  }

  // Laser harp
  else if(selected_option == options[1])
  {
    Serial.write("h");
  }

  // RFID
  else if(selected_option == options[2])
  {
    Serial.write("i");
  }

  // Temp/Humidity sensor
  else if(selected_option == options[3])
  {
    humidity_mode = true;
  }

  // Distance sensor
  else if(selected_option == options[4])
  {
    Serial.write("d");
  }

  // Touchpad
  else if(selected_option == options[5])
  {
    Serial.write("p");
  }

  // Big red switch
  else if(selected_option == options[6])
  {
    Serial.write("s");
  }
}

// read external read_onboard_LCD_buttons
// int read_external_LCD_buttons()
// {
//   int up = HIGH;
//   int click = HIGH;
//   int down = HIGH;
//
//   up = digitalRead(up_input);
//   click = digitalRead(click_input);
//   down = digitalRead(down_input);
//
//   static int prev_up_state = up;
//   static int prev_click_state = click;
//   static int prev_down_state = down;
//
//   if(click == LOW && prev_click_state == HIGH)
//   {
//     return btnSELECT;
//   }
//   else if(down == LOW && prev_down_state == HIGH)
//   {
//     return btnDOWN;
//   }
//   else if(up == LOW && prev_up_state == HIGH)
//   {
//     return btnUP;
//   }
//   else
//   {
//     return btnNONE;
//   }
//
//   prev_up_state = up;
//   prev_click_state = click;
//   prev_down_state = down;
// }
//
void read_external_LCD_buttons()
{
  int up = digitalRead(up_input);
  int down = digitalRead(down_input);
  if(up == LOW && down == HIGH)
  {
    btnClicked = btnUP;
  }
  else if(up == HIGH && down == LOW)
  {
    btnClicked = btnDOWN;
  }
  else if(up == LOW && down == LOW)
  {
    btnClicked = btnSELECT;
  }
  else
  {
    btnClicked = btnNONE;
  }
}

int read_onboard_LCD_buttons()
{
 int adc_key_in = analogRead(0);      // read the value from the sensor
 // my buttons when read are centered at these values: 0, 144, 329, 504, 741
 // we add approx 50 to those values and check to see if we are close

 if (adc_key_in < 50)   return btnRIGHT;
 if (adc_key_in < 195)  return btnUP;
 if (adc_key_in < 380)  return btnDOWN;
 if (adc_key_in < 555)  return btnLEFT;
 if (adc_key_in < 790)  return btnSELECT;



 return btnNONE;  // when all others fail, return this...
}
