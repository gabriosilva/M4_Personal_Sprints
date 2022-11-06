// All definitions here
#define LIGHT_SENSOR_PIN 2
#define STORE_BUTTON_PIN 35
#define PLAY_BUTTON_PIN 36
#define BUZZER_PIN 17

// Define all variables here

// stores all the led pins
int ledPins[4] = { 15, 7, 6, 5 };

// stores all measurements (in binary)
int measureVector[20] = {};
int measureCount = 0;

// auxiliary vector for converting to binary
int currentBinaryMeasurement[4] = { 0, 0, 0, 0 };

// vector for the melody list
int melodyList[15] = { 988, 1047, 1109, 1175, 1245, 1319, 1397, 1480, 1568, 1661, 1760, 1865, 1976, 2093, 2217 };

// booleans for controlling button pressed states
bool isStorePressed = 0;
bool isPlayPressed = 0;


// Setup all pins, inputs and outputs
void setup() {
  Serial.begin(115200);
  // led pins
  pinMode(ledPins[0], OUTPUT);
  pinMode(ledPins[1], OUTPUT);
  pinMode(ledPins[2], OUTPUT);
  pinMode(ledPins[3], OUTPUT);

  // button pins
  pinMode(STORE_BUTTON_PIN, INPUT_PULLUP);
  pinMode(PLAY_BUTTON_PIN, INPUT_PULLUP);

  // light sensor pin
  pinMode(LIGHT_SENSOR_PIN, INPUT);

  // buzzer pin
  pinMode(BUZZER_PIN, OUTPUT);

  // increases the default reading resolution
  // for analogic inputs
  analogReadResolution(12);
}






void loop() {
  // Changes button pressed states

  // If the store button state is LOW, changes the "store button" pressed state
  if (digitalRead(STORE_BUTTON_PIN) == LOW) {
    isStorePressed = !isStorePressed;
    delay(100);
  }

  // If the play button state is LOW, changes the "play button" pressed state
  if (digitalRead(PLAY_BUTTON_PIN) == LOW) {
    isPlayPressed = !isPlayPressed;
    delay(100);
  }

  // Call all button handlers accordingly

  // "store button" handler call
  if (isStorePressed) {
    storeButtonHandler();
    isStorePressed = false;
  }
  
  // "play button" handler call
  if (isPlayPressed) {
    playButtonHandler();
    isPlayPressed = false;
  }
  delay(100);
}


// Button handlers #####################################################################################################

// "store button" press handler
void storeButtonHandler() {
  // returns the light measurement converted to the 0-15 scale
  int lightMeasurement = measureLight();

  // adds the measurement to an int global vector
  // and increases the measurement counter
  appendVector(lightMeasurement, 20, measureCount, measureVector);
  measureCount++;

  // converts the current measurement to binary
  // and lights the input leds accordingly
  convertToBinary(lightMeasurement);
  binaryLed(currentBinaryMeasurement, ledPins);

  // play current's binary representation sound
  // from a melody vector, that ranges from 0 to 15
  playSound(BUZZER_PIN, melodyList[lightMeasurement], 300);
}


// "play button" press handler
void playButtonHandler() {

  // for each measurement, shows its led binary representation and
  // plays the its melody on a buzzer
  for (int i = 0; i < measureCount; i++) {
    int measure = measureVector[i];
    convertToBinary(measure);
    binaryLed(currentBinaryMeasurement, ledPins);
    playSound(BUZZER_PIN, melodyList[measure], 300);

    // resets the current vector element after executing
    // the first processes
    measureVector[i] = 0;
  };

  // resets the measurement count
  measureCount = 0;
}


// function that plays a frequency in a buzzer
void playSound(int buzzer_pin, int frequency, int duration) {
  tone(buzzer_pin, frequency, duration);
  delay(duration * 1.30);
  noTone(buzzer_pin);
}

// function that converts a number to binary,
// and replaces the global "currentBinaryMeasurement" vector elements
void convertToBinary(int number) {
  int i = 0, num = number;

  int count = 0;
  // Until all the four digits are fulfilled

  while (count < 4) {
    currentBinaryMeasurement[i] = number % 2;
    i++;
    number = number / 2;
    count++;
  }
}

// All auxiliary functions #############################################################################################

// updates and inserts elements into a vector
int *appendVector(int element, int maxValue, int lastPos, int Numbers[]) {
  int nextPos;
  nextPos = lastPos + 1;
  if ((element) > maxValue) {
    printf("Error: valorMax: %d, lastPos + 1 (index): %d\n", maxValue, nextPos);
  } else {
    Numbers[nextPos] = element;
  }
  return Numbers;
}

// Converts the light variation range to a predefined limit
int convertToRange(int lightValue, int maxLightValue, int maxRange) {
  return (lightValue * maxRange) / maxLightValue;
}


// reads the light intensity from the LDR sensor
// and returns it within a 0 to 15 range
int measureLight() {
  int analogValue = analogRead(LIGHT_SENSOR_PIN);
  int convertedLightValue = convertToRange(analogValue, 4095, 15);
  return convertedLightValue;
}

// Receives a binary digit vector
// And turns the declared leds on/off according to each vector element
void binaryLed(int binaryNumber[4], int pinVector[4]) {
  for (int i = 0; i < 4; i++) {
    if (binaryNumber[i] == 1) {
      digitalWrite(pinVector[i], HIGH);
    } else {
      digitalWrite(pinVector[i], LOW);
    }
    Serial.println(binaryNumber[i]);
  }
}
