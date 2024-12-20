// Definieer de analoge pinnen
const int analogPins[8] = {A0, A3, A6, A7, A4, A5, A18, A19};

void setup() {
  // Start de seriële communicatie
  Serial.begin(115200);
  
  // Configureer de analoge pinnen als ingangen
  for (int i = 0; i < 8; i++) {
    pinMode(analogPins[i], INPUT);
  }
}

void loop() {
  Serial.print("Values: ");
  // Lees en print de waarden van de analoge pinnen
  for (int i = 0; i < 8; i++) {
    int analogValue = analogRead(analogPins[i]);
    Serial.print(analogValue);
    Serial.print(" ");
  }
  Serial.print("\n");


  // Wacht een seconde voor de volgende meting
  delay(10);
}
