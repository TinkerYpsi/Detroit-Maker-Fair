#include <TT_LiquidCrystal.h>
#include <cactus_io_AM2302.h>
#include <SoftwareSerial.h>

// define some values used by the panel and buttons
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

int humidity_pin = 2;
int mega_output = 13;
int i = 0;
String selected_option;
String prev_selected_option;
bool humidity_mode = false;
int btnClicked = btnNONE;

String options[7] = {"Race", "Laser Harp", "RFID", "Temp/Humidity", "Distance sensor", "Big Red Switch", "Joystick"};

int read_onboard_LCD_buttons();
void switchMode(String selected_option);
void read_external_LCD_buttons();

TT_LiquidCrystal lcd(8, 9, 4, 5, 6, 7);      //default pins for the Arduino Uno
AM2302 dht(humidity_pin);

const int SERIAL_TX_PORT = 13;
const int SERIAL_RX_PORT = 0;
const int SERIAL_TX_PORT2 = 11;
const int SERIAL_RX_PORT2 = 12;
SoftwareSerial mySerial(SERIAL_RX_PORT, SERIAL_TX_PORT);
SoftwareSerial mySerial2(SERIAL_RX_PORT2, SERIAL_TX_PORT2);

void setup()
{
  Serial.begin(115200);
  pinMode(humidity_pin, INPUT_PULLUP);  
  pinMode(mega_output, OUTPUT);
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print(options[0]);
  Serial.println(options[0]);
  lcd.cursor();
  mySerial.begin(9600);
  mySerial2.begin(9600);
}

void loop()
{

  if (mySerial2.available()) {
    delay(10);
    char in_chars[50] = {'\0'};
    int i = 0;
    while (mySerial2.available()) {
      in_chars[i] = mySerial2.read();
      i++;
    }
    Serial.print("Incoming message: ");
    String in_str = String(in_chars);
    Serial.println(in_str);

    if (in_str.startsWith("u")) {
      btnClicked = btnUP;
    }
    else if (in_str.startsWith("d")) {
      btnClicked = btnDOWN;
    }
    else if (in_str.startsWith("s")) {
      btnClicked = btnSELECT;
    }
  }

  switch (btnClicked)
  {
    case btnUP:
      {
        humidity_mode = false;
        lcd.clear();
        // if i is less than size of options array
        if (i + 1 < sizeof(options) / sizeof(String))
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
        break;
      }
    case btnDOWN:
      {
        humidity_mode = false;
        lcd.clear();
        if (i > 0)
        {
          lcd.print(options[i - 1]);
          Serial.println(options[i - 1]);
          i--;
        }
        else
        {
          // options[i] is last element
          i = sizeof(options) / sizeof(String) - 1;
          lcd.print(options[i]);
        }
        lcd.cursor();
        break;
      }
    case btnSELECT:
      {
        humidity_mode = false;
        lcd.noCursor();
        selected_option = options[i];
        break;
      }
    case btnNONE:
      {
        // do nothing
        break;
      }      
  }
  btnClicked = btnNONE;

  if (prev_selected_option != selected_option)
  {
    switchMode(selected_option);
    prev_selected_option = selected_option;
  }

  static long last_check = millis();
  static bool clearedScreen = false;
  if (humidity_mode)
  {
    if (!clearedScreen)
    {
      lcd.clear();
      clearedScreen = true;
    }
    if (millis() - last_check > 3000)
    {
      last_check = millis();
      Serial.print("last check: ");
      Serial.println(last_check);
      Serial.print("time: ");
      Serial.println(millis());
      dht.readHumidity();
      dht.readTemperature();

      // Check if any reads failed and exit early (to try again).
      if (isnan(dht.humidity) || isnan(dht.temperature_C)) {
        Serial.println("DHT sensor read failure!");
        return;
      }

      if (btnClicked != btnNONE)
      {
        humidity_mode = false;
      }

      lcd.setCursor(0, 0);  // set cursor to 1st column, 1st row
      lcd.print("Humidity: ");
      lcd.print(dht.humidity); lcd.print("%");
      Serial.print(dht.humidity); Serial.print(" %\t\t");
      lcd.setCursor(0, 1);  // set cursor to 1st column, 2nd row
      lcd.print("Temp: ");
      lcd.print(dht.temperature_F); lcd.print("F");
      Serial.print(dht.temperature_F); Serial.print(" *F\n");
    }
  }

}

void switchMode(String selected_option)
{
  // Race
  if (selected_option == options[0])
  {
    mySerial.write("r");
  }

  // Laser harp
  else if (selected_option == options[1])
  {
    mySerial.write("h");
  }

  // RFID
  else if (selected_option == options[2])
  {
    mySerial.write("i");
  }

  // Temp/Humidity sensor
  else if (selected_option == options[3])
  {
    humidity_mode = true;
  }

  // Distance sensor
  else if (selected_option == options[4])
  {
    mySerial.write("d");
  }

  // Big red switch
  else if (selected_option == options[5])
  {
    mySerial.write("s");
  }

  // Joystick
  else if (selected_option == options[6])
  {
    mySerial.write("j");
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
