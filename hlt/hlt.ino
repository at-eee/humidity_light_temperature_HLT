#include <DHT.h>

#define DHTTYPE DHT11   // DHTTYPE as macro for DHT11
DHT dht(DHTPIN, DHTTYPE); // Creating the DHT object

// Named accoring to role Arduino I/O pins:
// Led segments pins:
#define LED_PIN_A 2
#define LED_PIN_B 3
#define LED_PIN_C 4
#define LED_PIN_D 5
#define LED_PIN_E 6
#define LED_PIN_F 7
#define LED_PIN_G 8
#define LED_PIN_DP 9
// 138 demultiplexer's selection pins (select lines pins):
#define DEMUX_PIN_0 11
#define DEMUX_PIN_1 12
#define DEMUX_PIN_2 13  // "Dummy" demux pin, unused for actual digit positions of LED display but used for "resetting" the LEDS ("resetLEDS()" function).
// Sensor-related pins:
#define DHTPIN 10       // DHT11 module pin
#define LDRPIN A0       // Light detection sensor pin (photoresistor's circuit electric current intensity reading pin)
#define BUTTON_PIN_1 A1

// Sensor/display modes:
#define TEMP_MODE 0  // Temperature mode
#define HUM_MODE 1   // Humidity mode
#define LIGHT_MODE 2 // Light level mode

// Special symbols:
#define EMPTY_DIGIT 10 // Empty "symbol".
#define HORIZONTAL_LINE 11 // horizontal line symbol (segment G activated only).

// Current, globally stored, digits to be displayed on a 4-digit LED display (where the digit0 is the least significant digit and digit3 is the most significant).
int digit3, digit2, digit1, digit0;

/*
Segments and their corresponding letter identifiers:

    A
 -------
 |     |
F|     |B
 |  G  |
 -------
 |     |
E|     |C
 |     |
 -------.<-DP
    D

Segments and their corresponding pins on 3461BS LED display:
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
  // Initialization of serial communication with the speed of 9600 bits per second (one of the popular data transfer speeds)
  Serial.begin(9600); 
  // DHT sensor initialization.
  dht.begin();         

  // LED display segment pins.
  pinMode(LED_PIN_A, OUTPUT);
  pinMode(LED_PIN_B, OUTPUT);
  pinMode(LED_PIN_C, OUTPUT);
  pinMode(LED_PIN_D, OUTPUT);
  pinMode(LED_PIN_E, OUTPUT);
  pinMode(LED_PIN_F, OUTPUT);
  pinMode(LED_PIN_G, OUTPUT);
  pinMode(LED_PIN_DP, OUTPUT);

  // Demultiplexer (demux) pins.
  pinMode(DEMUX_PIN_0, OUTPUT);
  pinMode(DEMUX_PIN_1, OUTPUT);
  pinMode(DEMUX_PIN_2, OUTPUT);

  // Input pin for button.
  pinMode(BUTTON_PIN_1, INPUT);
}

// Definition of powered segments for 0-9 digits and special symbols (hence 12 and not only 10 "digits"/array size).
const byte digits[12][8] = {
  // Segments: LOW = ON, HIGH = OFF (it's caused because of the internal structure and type of connection (Common Anode) of the LED display).
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
  {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH}, // "Special" digit ("empty" symbol)
  {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, LOW, HIGH} // "Special" digit (horizontal line (only segment G is active))
};


// "Resets" the LED display (all digit positions) (i.e: just displays nothing on the LED display) by powering the "non-existing" digit position.
// --
void resetLEDS(){
  digitalWrite(DEMUX_PIN_2, HIGH);
}

// Calculates (and assign to globally stored) digit values to be displayed on LED display - from the given float value.
void calculateDigits(float value_float){
  // Conversion of value from float to int.
  int value_int = (int)(value_float * 100);
  // Taking modulo 10, and diving by 10 to get the next digit of the number.
  digit0 = value_int % 10;
  value_int /= 10;
  digit1 = value_int % 10;
  value_int /= 10;
  digit2 = value_int % 10;
  value_int /= 10;
  digit3 = value_int % 10;
}

// Loop related to checking the temperature (temperature measuring/check mode)
void temperatureLoop(){

  int userIntercation = -1;

  for( ; ; ){

    float temperature = dht.readTemperature();

    if (isnan(temperature)) {
      Serial.println("Error reading temperature DHT11!");
      return;
    }

    calculateDigits(temperature);

    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" °C");

    userIntercation = displayAllDigits(TEMP_MODE);
    resetLEDS();

    if(userIntercation == 0){ // If user pressed the button (because userInteraction is set to 0 (earlier displayAllDigits returns 0), when user presses the button).
      break; // Leave this loop.
    } // Else continue the loop and take another measurement.

  }

}

// Loop related to checking the humidity level (level of humidity measuring/check mode)
void humidityLoop(){

  int userIntercation = -1;

  for( ; ; ){

    float humidity = dht.readHumidity();
    if (isnan(humidity)) {
      Serial.println("Error reading humidity from DHT11!");
      return;
    }

    calculateDigits(humidity);

    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");

    userIntercation = displayAllDigits(TEMP_MODE);
    resetLEDS();

    if(userIntercation == 0){ // If user pressed the button (because userInteraction is set to 0 (earlier displayAllDigits returns 0), when user presses the button).
      break; // Leave this loop.
    } // Else continue the loop and take another measurement.
  }
}

// --
// Loop related to checking the light level (level of brightness measuring/check mode).
void lightLevelLoop(){

  int userIntercation = -1;

  for( ; ; ){

    int lightLevel = analogRead(LDRPIN);

    // Printing on serial monitor
    Serial.print("Light level: ");
    Serial.print(lightLevel);
    if (lightLevel > 1000) Serial.println(" (Very bright)"); 
    else if (lightLevel > 750) Serial.println(" (Bright)"); 
    else if (lightLevel > 500) Serial.println(" (Medium brightness)"); 
    else if (lightLevel > 250) Serial.println(" (Dim)"); 
    else Serial.println(" (Very dim)"); 

    calculateLightLevelBar(lightLevel);
    userIntercation = displayAllDigits(LIGHT_MODE);
    resetLEDS();

    if(userIntercation == 0){ // If user pressed the button (because userInteraction is set to 0 (earlier displayAllDigits returns 0), when user presses the button).
      break; // Leave this loop.
    } // Else continue the loop and take another measurement.
  }
}

// Function displaying chosen digit on chosen position in chosen display mode.
void displayDigit(int digit, int position, int displayMode) {

  /*
  00 - digit position 0
  01 - digit position 1
  10 - digit position 2
  11 - digit position 3
  (We count from right to left side of the LED display (i.e: from least significant to most significant digit).
  */

  // Powering the specific (digit) position on LED display by operating on the demultiplexer (demux) pins.
  switch(position){
    case 0: // Right-most digit position on LED display
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
    case 3: // Left-most digit position on LED display
      digitalWrite(DEMUX_PIN_0, HIGH);
      digitalWrite(DEMUX_PIN_1, HIGH);
      break;
  }

  // Włączenie segmentów cyfry dla danej pozycji
  // Setting all segments of current digit position to corresponding symbol (digit) that is bound to be displayed.
  for (int i = 0; i < 8; i++) {
    digitalWrite(LED_PIN_A + i, digits[digit][i]);
  }

  // Setting the Decimal Point segment (DP), for specific display modes.
  if(displayMode == TEMP_MODE || displayMode == HUM_MODE){
    if(position == 2) digitalWrite(LED_PIN_DP, LOW);
  }

}

int displayAllDigits(int displayMode){

  const int flickerRate = 5; // Flicker (change displayed (powered) digit on LED display) every 5ms (milliseconds).
  const int timeLimit = 200; // After 200ms automatically stop and take another measurement.
  int i = 0; // Variable used for iteration counting.
  digitalWrite(DEMUX_PIN_2, LOW); // Making sure this pin is set to low (after the resetLEDS() call).
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
      return 0; // Return 0, if user wants to end measurements in the current mode and switch to the next one (user pressed the button).
    }else if(i * 4 * flickerRate >= timeLimit){
      return 1; // Return 1, if time of displaying the current measurement has passed and next measurement shall be taken (timeLimit has passed).
    }

  }

}

void calculateLightLevelBar(int lightLevel) {
  
  if (lightLevel > 1000) { 
    // Very high brightness -> Displays '----' (4 bars)
	  digit3 = HORIZONTAL_LINE;
    digit2 = HORIZONTAL_LINE;
    digit1 = HORIZONTAL_LINE;
    digit0 = HORIZONTAL_LINE;
  } 
  else if (lightLevel > 750) { 
    // High brightness -> Displays '---'
	  digit3 = EMPTY_DIGIT;
    digit2 = HORIZONTAL_LINE;
    digit1 = HORIZONTAL_LINE;
    digit0 = HORIZONTAL_LINE;
  } 
  else if (lightLevel > 500) { 
    // Medium brightness -> Displays '--'
	  digit3 = EMPTY_DIGIT;
    digit2 = EMPTY_DIGIT;
    digit1 = HORIZONTAL_LINE;
    digit0 = HORIZONTAL_LINE;
  } 
  else if (lightLevel > 250) { 
    // Low brightness -> Displays '-'
	  digit3 = EMPTY_DIGIT;
    digit2 = EMPTY_DIGIT;
    digit1 = EMPTY_DIGIT;
    digit0 = HORIZONTAL_LINE;
  } else { 
    // Very low brightness -> Displays '' (nothing) (0 bars)
	  digit3 = EMPTY_DIGIT;
    digit2 = EMPTY_DIGIT;
    digit1 = EMPTY_DIGIT;
    digit0 = EMPTY_DIGIT;
  }

}

// Main program loop
void loop() {

  temperatureLoop();

  delay(250); // In order to make sure button press isn't read as multiple presses.

  humidityLoop();

  delay(250);

  lightLevelLoop();

  delay(250);

}
