#include <DHT.h>

#define DHTPIN 10       // Pin dla modułu DHT11
#define DHTTYPE DHT11   // Typ czujnika DHT11
DHT dht(DHTPIN, DHTTYPE); // Tworzenie obiektu DHT

#define LDRPIN A0      // Pin dla czujnika światła

// Konkretnie nazwane piny w Arduino
#define LED_PIN_A 2
#define LED_PIN_B 3
#define LED_PIN_C 4
#define LED_PIN_D 5
#define LED_PIN_E 6
#define LED_PIN_F 7
#define LED_PIN_G 8
#define LED_PIN_DP 9
#define DEMUX_PIN_0 11
#define DEMUX_PIN_1 12
#define DEMUX_PIN_2 13
#define BUTTON_PIN_1 A1

//Tryby wyświetlania (dla funkcji displayDigit i displayAllDigits)
#define TEMP_MODE 0  //Tryb temperatury
#define HUM_MODE 1   //Tryb wilgotności
#define LIGHT_MODE 2 //Tryb poziomu światła

//"Specjalne" symbole cyfr
#define EMPTY_DIGIT 10 // "Pusta" cyfra
#define HORIZONTAL_LINE 11 // Pozioma kreska

// Aktualne wartości cyfr do wyświetlenia po obliczeniach
int digit3, digit2, digit1, digit0;

/*
Które segmenty odpowiadają którym pinom na naszym 4-cyfrowym 8-segmentowym wyświetlaczu LED.
A 11
B 7
C 4
D 2
E 1
F 10
G 5
DP 3
*/

void setup() {
  // Inicjalizacja komunikacji szeregowej z prędkością 9600 baudów => przesyłanie 9600 bitów na sekundę (jedna z popularnych prędkości transmisji danych)
  Serial.begin(9600); 
  // Inicjalizacja czujnika DHT 
  dht.begin();         

  // Piny segmentów wyświetlacza LED
  pinMode(LED_PIN_A, OUTPUT);
  pinMode(LED_PIN_B, OUTPUT);
  pinMode(LED_PIN_C, OUTPUT);
  pinMode(LED_PIN_D, OUTPUT);
  pinMode(LED_PIN_E, OUTPUT);
  pinMode(LED_PIN_F, OUTPUT);
  pinMode(LED_PIN_G, OUTPUT);
  pinMode(LED_PIN_DP, OUTPUT);

  // Piny demultipleksera (demux)
  pinMode(DEMUX_PIN_0, OUTPUT);
  pinMode(DEMUX_PIN_1, OUTPUT);
  pinMode(DEMUX_PIN_2, OUTPUT);

  // Pin wejściowy przycisku
  pinMode(BUTTON_PIN_1, INPUT);
}

// Definicja segmentów dla cyfr 0-9
const byte digits[12][8] = {
  // Segmenty: LOW = WŁĄCZONY, HIGH = WYŁĄCZONY
  {LOW, LOW, LOW, LOW, LOW, LOW, HIGH, HIGH},  // 0
  {HIGH, LOW, LOW, HIGH, HIGH, HIGH, HIGH, HIGH}, // 1
  {LOW, LOW, HIGH, LOW, LOW, HIGH, LOW, HIGH},  // 2
  {LOW, LOW, LOW, LOW, HIGH, HIGH, LOW, HIGH},  // 3
  {HIGH, LOW, LOW, HIGH, HIGH, LOW, LOW, HIGH}, // 4
  {LOW, HIGH, LOW, LOW, HIGH, LOW, LOW, HIGH},  // 5
  {LOW, HIGH, LOW, LOW, LOW, LOW, LOW, HIGH},  // 6
  {LOW, LOW, LOW, HIGH, HIGH, HIGH, HIGH, HIGH}, // 7
  {LOW, LOW, LOW, LOW, LOW, LOW, LOW, HIGH},  // 8
  {LOW, LOW, LOW, HIGH, HIGH, LOW, LOW, HIGH},  // 9
  {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH}, // "Specjalna" cyfra ("pusty" symbol)
  {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, LOW, HIGH} // "Specjalna" cyfra (pozioma kreska)
};

// "Resetuje" diody LED poprzez zasilanie nieistniejącej pozycji (nie zasila żadnej z obecnych pozycji)
void resetLEDS(){
  digitalWrite(DEMUX_PIN_2, HIGH);
}


// Funkcja wyświetlająca cyfrę na danej pozycji na wyświetlaczu
void displayDigit(int digit, int position, int displayMode) {

  // Zasilanie danej pozycji cyfry na wyświetlaczu LED
  switch(position){
    case 0: // Prawa skrajna pozycja cyfry na wyświetlaczu
      digitalWrite(DEMUX_PIN_0, LOW);
      digitalWrite(DEMUX_PIN_1, LOW);
      break;
    case 1:
      digitalWrite(DEMUX_PIN_0, HIGH);
      digitalWrite(DEMUX_PIN_1, LOW);
      break;
    case 2:
      digitalWrite(DEMUX_PIN_0, LOW);
      digitalWrite(DEMUX_PIN_1, HIGH);
      break;
    case 3: // Lewa skrajna pozycja cyfry na wyświetlaczu
      digitalWrite(DEMUX_PIN_0, HIGH);
      digitalWrite(DEMUX_PIN_1, HIGH);
      break;
  }

  // Włączenie segmentów cyfry dla danej pozycji
  for (int i = 0; i < 8; i++) {
    digitalWrite(LED_PIN_A + i, digits[digit][i]);
  }

  // Ustawienie kropki dziesiętnej
  if(displayMode == TEMP_MODE || displayMode == HUM_MODE){
    if(position == 2) digitalWrite(LED_PIN_DP, LOW);
  }

}

void displayAllDigits(int displayMode){ // "Nieskończony" czas trwania (dopóki użytkownik nie naciśnie przycisku)

  digitalWrite(DEMUX_PIN_2, LOW); // Upewnienie się, że nie zostanie wybrana nieistniejąca pozycja (po resecie resetLEDS())
  const int flickerRate = 5; /// Migotanie co 5 ms
  for ( ; ; ) { 

    displayDigit(digit0, 0, displayMode);
    delay(flickerRate);

    displayDigit(digit1, 1, displayMode);
    delay(flickerRate);

    displayDigit(digit2, 2, displayMode);
    delay(flickerRate);
	
	  displayDigit(digit3, 3, displayMode);
    delay(flickerRate);

    // Jeśli użytkownik naciśnie przycisk
    if(digitalRead(BUTTON_PIN_1)){
      break;
    }

  }

  resetLEDS();
}

void displayLightLevel(int lightLevel) {
  
  if (lightLevel > 750) { 
    // Bardzo duża jasność -> Wyświetla '----' (cztery kreski)
	  digit3 = HORIZONTAL_LINE;
    digit2 = HORIZONTAL_LINE;
    digit1 = HORIZONTAL_LINE;
    digit0 = HORIZONTAL_LINE;
  } 
  else if (lightLevel > 500) { 
    // Duża jasność -> Wyświetla '---'
	  digit3 = EMPTY_DIGIT;
    digit2 = HORIZONTAL_LINE;
    digit1 = HORIZONTAL_LINE;
    digit0 = HORIZONTAL_LINE;
  } 
  else if (lightLevel > 250) { 
    // Średnia jasność -> Wyświetla '--'
	  digit3 = EMPTY_DIGIT;
    digit2 = EMPTY_DIGIT;
    digit1 = HORIZONTAL_LINE;
    digit0 = HORIZONTAL_LINE;
  } 
  else { 
    // Niska jasność -> Wyświetla '-'
	  digit3 = EMPTY_DIGIT;
    digit2 = EMPTY_DIGIT;
    digit1 = EMPTY_DIGIT;
    digit0 = HORIZONTAL_LINE;
  }

  displayAllDigits(LIGHT_MODE);
}

void loop() {

  // Odczyt temperatury z czujnika DHT11
  float temp = dht.readTemperature();
  if (isnan(temp)) {
    Serial.println("Error reading DHT11!");
    return;
  }

  // Konwersja temperatury na wartość całkowitą
  int temp_int = (int)(temp * 100);
  digit3 = (temp_int / 1000) % 10;   // Pierwsza cyfra (np., 2 dla 20.81)
  digit2 = (temp_int / 100) % 10;    // Druga cyfra (np., 0 dla 20.81)
  digit1 = (temp_int / 10) % 10;     // Trzecia cyfra (np., 8 dla 20.81)
  digit0 = temp_int % 10;		         // Czwarta cyfra (np., 1 dla 20.81)

  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.println(" °C");

  delay(250); // Oczekiwanie, aby upewnić się, że przycisk nie zostanie ponownie wciśnięty przed rozpoczęciem kolejnego trybu
  displayAllDigits(TEMP_MODE);

  float hum = dht.readHumidity();
  if (isnan(hum)) {
    Serial.println("Error reading DHT11!");
    return;
  }
  
  int hum_int = (int)(hum * 100);
  digit3 = (hum_int / 1000) % 10;
  digit2 = (hum_int / 100) % 10;
  digit1 = (hum_int / 10) % 10;
  digit0 = hum_int % 10;

  Serial.print("Humidity: ");
  Serial.print(hum);
  Serial.println(" %");

  delay(250); 
  displayAllDigits(HUM_MODE);

  int lightLevel = analogRead(LDRPIN);
  Serial.print("Light level: ");
  Serial.println(lightLevel);

  delay(250);

  displayLightLevel(lightLevel);
}
