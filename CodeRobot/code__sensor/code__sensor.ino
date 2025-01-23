#include <SoftwareSerial.h>

// Pin definitions
#define TOUCH_SENSOR_TOP 2
#define TOUCH_SENSOR_DOWN 3
#define TOUCH_SENSOR_RIGHT 4
#define TOUCH_SENSOR_LEFT 5
#define TOUCH_SENSOR_CENTER 6

// Threshold and time window for tickling
const int TICKLE_THRESHOLD = 10; // Minimum combined activations for a "tickle"
const unsigned long TICKLE_TIME = 1000; // Time window in milliseconds

// Variables to track activations and time
int totalTouches = 0; // Global counter for combined activations
unsigned long lastTickleTime = 0;

// Sensor order: TOP, DOWN, RIGHT, LEFT, CENTER
int sensorPins[5] = {TOUCH_SENSOR_TOP, TOUCH_SENSOR_DOWN, TOUCH_SENSOR_RIGHT, TOUCH_SENSOR_LEFT, TOUCH_SENSOR_CENTER};

// MP3 module pins
#define MP3_RX 10
#define MP3_TX 11

// Initialize software serial for MP3 module
SoftwareSerial mp3Serial(MP3_RX, MP3_TX);

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(9600);

  // Initialize MP3 module
  mp3Serial.begin(9600);

  // Set sensor pins as inputs
  for (int i = 0; i < 5; i++) {
    pinMode(sensorPins[i], INPUT);
  }
}

void loop() {
  // Current time for tracking
  unsigned long currentTime = millis();

  // Check each sensor and count activations
  for (int i = 0; i < 5; i++) {
    if (digitalRead(sensorPins[i]) == HIGH) {
      totalTouches++;
    }
  }

  // If time window has passed, evaluate and reset
  if (currentTime - lastTickleTime >= TICKLE_TIME) {
    if (totalTouches >= TICKLE_THRESHOLD) {
      // A tickle was detected
      Serial.println("Tickle detected!");
      Serial.println("Playing sound...");
      // Send command to play the same sound
      // Example: Replace this with your MP3 module command
    }

    // Reset the counter and time
    totalTouches = 0;
    lastTickleTime = currentTime;
  }
}
