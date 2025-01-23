
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

// Pin definitions
#define TOUCH_SENSOR_TOP 2
#define TOUCH_SENSOR_DOWN 3
#define TOUCH_SENSOR_RIGHT 4
#define TOUCH_SENSOR_LEFT 5
#define TOUCH_SENSOR_CENTER 6

SoftwareSerial Songs_Serial(9, 8); //tx, rx
SoftwareSerial Laugh_Serial(12, 11); //tx, rx

// Create the Player object
DFRobotDFPlayerMini Songs_player;
DFRobotDFPlayerMini Laugh_player;

bool hasPlayed = false;

// Threshold and time window for tickling
const int TICKLE_THRESHOLD = 10; // Minimum combined activations for a "tickle"
const unsigned long TICKLE_TIME = 1000; // Time window in milliseconds

// Variables to track activations and time
int totalTouches = 0; // Global counter for combined activations
unsigned long lastTickleTime = 0;

// Sensor order: TOP, DOWN, RIGHT, LEFT, CENTER
int sensorPins[5] = {TOUCH_SENSOR_TOP, TOUCH_SENSOR_DOWN, TOUCH_SENSOR_RIGHT, TOUCH_SENSOR_LEFT, TOUCH_SENSOR_CENTER};


void setup() {
  // Initialize serial communication for debugging
  Serial.begin(19200);

  // Set sensor pins as inputs
  for (int i = 0; i < 5; i++) {
    pinMode(sensorPins[i], INPUT);
  }

  Laugh_Serial.begin(9600);
  if (Laugh_player.begin(Laugh_Serial)) {
    Serial.println("Laugh_player OK");

    // Set volume to maximum (0 to 30).
    Laugh_player.volume(3); //30 is very loud
  } else {
    Serial.println("Connecting to Laugh DFPlayer Mini failed!");
  }

  Songs_Serial.begin(9600);
  if (Songs_player.begin(Songs_Serial)) {
    Serial.println("Songs_player OK");

    // Set volume to maximum (0 to 30).
    Songs_player.volume(3); //30 is very loud
  } else {
    Serial.println("Connecting to Songs DFPlayer Mini failed!");
  }
}

void loop() 
{
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
      if (!hasPlayed) {
        Serial.println("Playing");
        Songs_player.play(1);
        Laugh_player.play(1);
        hasPlayed = true; // Marquer que les chansons ont été jouées
      }
    }

    // Reset the counter and time
    totalTouches = 0;
    lastTickleTime = currentTime;
  }
}