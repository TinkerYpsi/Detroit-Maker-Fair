int button = 5;

void setup()
{
  pinMode(button, OUTPUT);
  digitalWrite(button, HIGH);
}

void loop()
{
  digitalWrite(button, LOW);
  delay(500);
  digitalWrite(button, HIGH);
  delay(2000);
}
