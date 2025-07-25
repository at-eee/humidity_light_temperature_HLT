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

// Tryby wyświetlania (dla funkcji displayDigit i displayAllDigits)
#define TEMP_MODE 0  // Tryb temperatury
#define HUM_MODE 1   // Tryb wilgotności
#define LIGHT_MODE 2 // Tryb poziomu światła

// "Specjalne" symbole cyfr
#define EMPTY_DIGIT 10 // "Pusta" cyfra
#define HORIZONTAL_LINE 11 // Pozioma kreska

// Aktualne, globalnie przechowywane wartości cyfr do wyświetlenia na wyświetlaczu LED
int digit3, digit2, digit1, digit0;

/*
Które segmenty odpowiadają którym pinom 4-cyfrowego 8-segmentowego wyświetlacza LED (firmy/model: ??? (- to do))
licząć od (??? - to do):
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
  // Segmenty: LOW = WŁĄCZONY, HIGH = WYŁĄCZONY (jest to spowodowane przez budowe i sposób działania wyświetlacza (wyświetlacz LED ze wspólną anodową))
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
// Dokładniej: Zgodnie z wykorzystaną ilością stanów i posiadanym demultiplekserem wciąż pozostają nam 4 stany, które 
// nie zostały wykorzystane/użyte, a zatem, które nie oświetlają żadnej z czterech pozycji ("cyfr") na naszym wyświetlaczu LED.
void resetLEDS(){
  digitalWrite(DEMUX_PIN_2, HIGH);
}

//Zamiast wskaźników, optymalniej będzie używać jednak nadal zmiennych globalnych
void calculateDigits(float temperature_float){
  //Po wprowadzonych zmianach powinno teoretycznie być bardziej efektywnie/optymalnie (mniej skomplikowane operacje).
  // Conversion of temperature from float to int.
  int temperature_int = (int)(temperature_float * 100); 
  // Zamieniamy liczbę z formatu (liczba zmiennoprzecinkowa) "ab.cdef" na format: "abcd" (liczba całkowita) (gdzie; a-f to cyfry)
  // (np. 20.81 -> 2081)
  digit0 = temperature_int % 10; // Czwarta cyfra (np., cyfra 1 z 208*1* (20.81))
  temperature_int /= 10;
  digit1 = temperature_int % 10; // Trzecia cyfra (np., cyfra 8 z 20*8*1 (20.81))
  temperature_int /= 10;
  digit2 = temperature_int % 10; // Druga cyfra (np., cyfra 0 z 2*0*81 (20.81))
  temperature_int /= 10;
  digit3 = temperature_int % 10; // Pierwsza cyfra (np., cyfra 2 z *2*081 (20.81))
}

// Pętla sprawdzająca temperature (tryb temperaturowy/tryb mierzenia temperatury)
void temperatureLoop(){

  int userIntercation = -1;

  for( ; ; ){

    float temperature = dht.readTemperature();

    if (isnan(temperature)) {
      Serial.println("Error reading temperature DHT11!");
      return;
    }

    // Konwersja temperatury na wartość całkowitą
    calculateDigits(temperature);

    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" °C");

    userIntercation = displayAllDigits(TEMP_MODE);
    resetLEDS();

    if(userIntercation == 0){ //jeżeli uzytkownik wcisnął przycisk (0 zwracane jest, gdy użytkownik wciśnie przycisk)
      break; // wyjdź z tej pętli
    } // W przeciwnym razie kontynuuj pętle normalnie robiąc kolejny pomiar.

  }

}

// Pętla sprawdzająca poziom wilgotności (tryb sprawdzania poziomu wilgotności)
void humidityLoop(){

  int userIntercation = -1;

  for( ; ; ){

    float humidity = dht.readHumidity();
    if (isnan(humidity)) {
      Serial.println("Error reading humidity from DHT11!");
      return;
    }

    // Konwersja temperatury na wartość całkowitą
    calculateDigits(humidity);

    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");

    userIntercation = displayAllDigits(TEMP_MODE);
    resetLEDS();

    if(userIntercation == 0){ //jeżeli uzytkownik wcisnął przycisk (0 zwracane jest, gdy użytkownik wciśnie przycisk)
      break; // wyjdź z tej pętli
    } // W przeciwnym razie kontynuuj pętle normalnie robiąc kolejny pomiar.
  }
}

// (Można teoretycznie później połączyć wspólne fragmenty w jedną funkcje (w której fragmenty są na przykład wybierane za sprawą switch-case'ów
// albo skorzystać ze wskaźnika funkcji) potem, ale trzeba ocenić opłacalność względem wydajności).
// Pętla sprawdzająca poziom naświetlenia (tryb sprawdzający poziom naświetlenia)
void lightLevelLoop(){

  int userIntercation = -1;

  for( ; ; ){

    int lightLevel = analogRead(LDRPIN);

    // Printing on serial monitor
    Serial.print("Light level: ");
    Serial.print(lightLevel);
    if (lightLevel > 1000) Serial.println(" (Bardzo wysoka)"); 
    else if (lightLevel > 750) Serial.println(" (Wysoka)"); 
    else if (lightLevel > 500) Serial.println(" (Średnia)"); 
    else if (lightLevel > 250) Serial.println(" (Niska)"); 
    else Serial.println(" (Bardzo niska)"); 

    calculateLightLevelBar(lightLevel);
    userIntercation = displayAllDigits(LIGHT_MODE);
    resetLEDS();

    if(userIntercation == 0){ //jeżeli uzytkownik wcisnął przycisk (0 zwracane jest, gdy użytkownik wciśnie przycisk)
      break; // wyjdź z tej pętli
    } // W przeciwnym razie kontynuuj pętle normalnie robiąc kolejny pomiar.
  }
}

// Funkcja wyświetlająca cyfrę na danej pozycji na wyświetlaczu
void displayDigit(int digit, int position, int displayMode) {

  /*
  00 - cyfra 0
  01 - cyfra 1
  10 - cyfra 2
  11 - cyfra 3
  (liczymy od skrajnej prawej strony wyświetlacza/liczby (cyfry mniej znaczące) w kierunku lewej (cyfry bardziej znaczące): cyfra 0, 1, itd.)
  */

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

int displayAllDigits(int displayMode){

  const int flickerRate = 5; /// Migotanie co 5 ms
  const int timeLimit = 200; // Po 200ms zmierz ponownie (automatycznie przerwij i mierz ponownie)
  int i = 0; //Zmienna do liczenia liczby iteracji
  digitalWrite(DEMUX_PIN_2, LOW); // Upewnienie się, że nie zostanie wybrana nieistniejąca pozycja (po resecie resetLEDS())
  for ( ; ; ) { 

    i++;

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
      return 0; // Zwracamy 0, gdy chcemy zakończyć pomiary z obecnego trybu i przejść do następnego (użytkownik wcisnął przycisk).
    }else if(i * 4 * flickerRate >= timeLimit){
      return 1; // Zwracamy 1, jeżeli czas pokazywania danego pomiaru minął i ma zostać wykonany ponowny pomiar dla tego trybu.
    }

  }

}

void calculateLightLevelBar(int lightLevel) {
  
  if (lightLevel > 1000) { 
    // Bardzo duża jasność -> Wyświetla '----' (4 kreski)
	  digit3 = HORIZONTAL_LINE;
    digit2 = HORIZONTAL_LINE;
    digit1 = HORIZONTAL_LINE;
    digit0 = HORIZONTAL_LINE;
  } 
  else if (lightLevel > 750) { 
    // Duża jasność -> Wyświetla '---'
	  digit3 = EMPTY_DIGIT;
    digit2 = HORIZONTAL_LINE;
    digit1 = HORIZONTAL_LINE;
    digit0 = HORIZONTAL_LINE;
  } 
  else if (lightLevel > 500) { 
    // Średnia jasność -> Wyświetla '--'
	  digit3 = EMPTY_DIGIT;
    digit2 = EMPTY_DIGIT;
    digit1 = HORIZONTAL_LINE;
    digit0 = HORIZONTAL_LINE;
  } 
  else if (lightLevel > 250) { 
    // Niska jasność -> Wyświetla '-'
	  digit3 = EMPTY_DIGIT;
    digit2 = EMPTY_DIGIT;
    digit1 = EMPTY_DIGIT;
    digit0 = HORIZONTAL_LINE;
  } else { 
    // Bardzo niska jasność -> Wyświetla '' (puste) (0 kresek)
	  digit3 = EMPTY_DIGIT;
    digit2 = EMPTY_DIGIT;
    digit1 = EMPTY_DIGIT;
    digit0 = EMPTY_DIGIT;
  }

}

// Główny loop jest teraz znacznie "czystszy".
void loop() {

  temperatureLoop();

  delay(250); // Dla upewnienia się, że pojedyncze wciśnięcie przycisku nie zostanie odczytane jako wielokortone.

  humidityLoop();

  delay(250);

  lightLevelLoop();

  delay(250);

}
