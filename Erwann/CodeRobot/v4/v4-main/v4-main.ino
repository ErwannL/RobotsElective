#include <Wire.h>
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

// I2C address for Arduino
#define SLAVE_ADDRESS 0x01  // Adjust this address as needed (e.g., 0x01, 0x02, etc.)

#define NUM_VALUES 5                    // Number of emotion values to handle
int state_machine_values[NUM_VALUES];   // Store received emotion values

byte change_in_happy = 0;               // Emotion value for happiness (range: 0 = sad, 1000 = happy)
byte change_in_energy = 0;              // Emotion value for energy (range: 0 = lethargic, 1000 = energetic)
byte change_in_friendly = 0;            // Emotion value for friendliness (range: 0 = hostile, 1000 = friendly)
byte change_in_assertive = 0;           // Emotion value for assertiveness (range: 0 = shy, 1000 = assertive)
byte change_in_delight = 0;             // Emotion value for delight (range: 0 = disgust, 1000 = delight)

// Pin definitions for touch sensors
#define TOUCH_SENSOR_TOP 2
#define TOUCH_SENSOR_DOWN 3
#define TOUCH_SENSOR_RIGHT 4
#define TOUCH_SENSOR_LEFT 5
#define TOUCH_SENSOR_CENTER 6

SoftwareSerial Laugh_Serial(9, 8); // Define software serial (tx, rx)

// Create the DFPlayer Mini object
DFRobotDFPlayerMini Laugh_player;

// Constants for tickling detection
const int TICKLE_THRESHOLD = 5; // Minimum combined activations to detect a "tickle"
const unsigned long TICKLE_TIME = 500; // Time window for tickling detection in milliseconds
const unsigned long NO_TICKLE_TIME = 5000; // Time window for no tickle detection
const unsigned long LAUGH = 15000; // Time window to play laugh

unsigned long last_played_laugh = 0; // Last time laugh was played

// Variables for touch sensor tracking
int totalTouches = 0; // Counter for total touch activations
unsigned long lastTickleTime = 0; // Last time tickling was detected

int sensorPins[5] = {TOUCH_SENSOR_TOP, TOUCH_SENSOR_DOWN, TOUCH_SENSOR_RIGHT, TOUCH_SENSOR_LEFT, TOUCH_SENSOR_CENTER};

bool player_ready = false;

// Setup function to initialize hardware and communication
void setup() {
  // Initialize emotion values with a neutral baseline
  state_machine_values[0] = 500;
  state_machine_values[1] = 500;
  state_machine_values[2] = 500;
  state_machine_values[3] = 500;
  state_machine_values[4] = 500;

  // Initialize I2C communication as slave
  Wire.begin(SLAVE_ADDRESS);
  Wire.onRequest(requestEvent);  // Register event handler for data request
  Wire.onReceive(receiveEvent);  // Register event handler for data reception

  // Initialize serial communication for debugging
  Serial.begin(9600);  

  // Set sensor pins as input
  for (int i = 0; i < 5; i++) {
    pinMode(sensorPins[i], INPUT);
  }

  Laugh_Serial.begin(9600);  // Start serial communication with the DFPlayer

  // Attempt to initialize the DFPlayer Mini
  for (int i = 0; i < 5; i++) {
    if (Laugh_player.begin(Laugh_Serial)) {
      Serial.println("Laugh_player OK");

      // Set volume level
      Laugh_player.volume(2);  // Adjust volume level (0 to 30)
      player_ready = true;
      break;
    } else {
      Serial.println("Connecting to Laugh DFPlayer Mini failed! Retrying...");
      delay(500);
    }
  }

  if (!player_ready) {
    Serial.println("Laugh_Serial initialization failed after 5 attempts.");
  }

  lastTickleTime = millis();
}

// Main loop function
void loop() {
  doSomething(); // Detect touches and handle emotions
  printStateMachineValues(); // Output current emotion values to the serial monitor
  delay(1000); // Arbitrary delay for the loop (adjust as needed)
}

// Detect touch sensor activations and evaluate emotions
void doSomething() {
  for (int i = 0; i < 5; i++) {
    if (digitalRead(sensorPins[i]) == HIGH) {
      totalTouches++;  // Increment activation count for each touch
    }
  }

  unsigned long currentTime = millis();  // Get current time

  // If the time window has passed, evaluate and reset tickle count
  if (currentTime - lastTickleTime >= TICKLE_TIME) {
    if (totalTouches >= TICKLE_THRESHOLD) {
      lastTickleTime = currentTime;
      totalTouches = 0;

      if ((state_machine_values[4] < 300) || (state_machine_values[2] < 300)) {
        change_in_friendly = changeState(change_in_friendly, -1);
        
        if ((state_machine_values[2] < 300) || (state_machine_values[4] < 300)) {
          last_played_laugh = playSongs(Laugh_player, 6, LAUGH, last_played_laugh);
        }

        if ((state_machine_values[2] < 200) || (state_machine_values[4] < 200)) {
          last_played_laugh = playSongs(Laugh_player, 7, LAUGH, last_played_laugh);
        }

        if ((state_machine_values[2] < 100) || (state_machine_values[4] < 100)) {
          last_played_laugh = playSongs(Laugh_player, 8, LAUGH, last_played_laugh);
        }
      } else {
        change_in_friendly = changeState(change_in_friendly, 1);
        change_in_happy = changeState(change_in_happy, 1);

        if (state_machine_values[0] > 700) {
          int randomLaugh = random(1, 6);  // Get a random laugh sound to play
          last_played_laugh = playSongs(Laugh_player, randomLaugh, LAUGH, last_played_laugh);
        }
      }
    } else {
      Serial.print("still ");
      Serial.print(TICKLE_THRESHOLD - totalTouches);
      Serial.println(" tickle before detected");
    }
  }

  sendToStateMachine(); // Send updated emotion values to state machine

  if (currentTime - lastTickleTime >= NO_TICKLE_TIME) {
    change_in_happy = changeState(change_in_happy, -1);  // Gradually decrease happiness over time
  }
}

// Print emotion values to the serial monitor
void printStateMachineValues() {
  const char* labels[NUM_VALUES] = {
    "Happiness_Sadness",
    "Energetic_Lethargic",
    "Friendly_Hostile",
    "Assertive_Shy",
    "Delight_Disgust"
  };

  const char* positive_labels[NUM_VALUES] = {
    "Happiness",
    "Energetic",
    "Friendly",
    "Assertive",
    "Delight"
  };

  const char* negative_labels[NUM_VALUES] = {
    "Sadness",
    "Lethargic",
    "Hostile",
    "Shy",
    "Disgust"
  };

  Serial.println("Emotions value:");

  for (int i = 0; i < NUM_VALUES; i++) {
    Serial.print("    ");
    Serial.print(labels[i]);
    Serial.print(" = ");

    if (state_machine_values[i] < 300) {
      Serial.print(negative_labels[i]);
    } else if (state_machine_values[i] <= 700) {
      Serial.print("Neutral");
    } else {
      Serial.print(positive_labels[i]);
    }

    Serial.print(" (");
    Serial.print(state_machine_values[i]);
    Serial.println(")");
  }
}

// Play the laugh sound based on the player readiness and timing
unsigned long playSongs(DFRobotDFPlayerMini &player, int music_number, const unsigned long reset_timer, unsigned long last_played) {
  if (player_ready) {
    unsigned long now = millis();
    unsigned long dif = now - last_played;

    if ((dif > reset_timer) || last_played == 0) {
      player.play(music_number);  // Play the selected laugh sound
      Serial.println("PLAY");
      return now;
    } else {
      return last_played;  // Return the last play time if it's too soon
    }
  }
  return last_played;  // Return last played time if player isn't ready
}

// Change the state of an emotion
byte changeState(byte state_to_change, int to_change) {
    if ((state_to_change == 1 && to_change <= 0) || (state_to_change == 0 && to_change >= 0)) {
      state_to_change += to_change;
    } else if (state_to_change == 1 && to_change == 1) {
      state_to_change = 1;
    } else if (state_to_change == 255 && to_change == 1) {
      state_to_change = 0;
    } else {
      state_to_change = 255;
    }
    return state_to_change;
}

// Send emotion changes to the state machine via I2C
void sendToStateMachine() {
  requestEvent();  // Trigger request event to send data

  Serial.print(change_in_happy);
  Serial.print(" ");
  Serial.print(change_in_energy);
  Serial.print(" ");
  Serial.print(change_in_friendly);
  Serial.print(" ");
  Serial.print(change_in_assertive);
  Serial.print(" ");
  Serial.println(change_in_delight);
}

// I2C event handlers for receiving and sending data
void requestEvent() {
  byte change[NUM_VALUES] = { change_in_happy, change_in_energy, change_in_friendly, change_in_assertive, change_in_delight };
  Wire.write(change, NUM_VALUES);  // Send emotion changes to master
}

void receiveEvent(int howMany) {
  if (howMany == NUM_VALUES * 2) {
    for (int i = 0; i < NUM_VALUES; ++i) {
      byte x1, x2;
      x1 = Wire.read();
      x2 = Wire.read();
      state_machine_values[i] = (int)x1 << 8 | (int)x2;
    }
  }
}
