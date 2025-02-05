#include <Wire.h>
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

// I2C address for the Arduino, change according to your setup
#define SLAVE_ADDRESS 0x01  // Change to 0x01, 0x02, etc. depending on the value chosen by your teammates

// Used for communicating emotion values
#define NUM_VALUES 5                    // Number of emotion values to be sent
int state_machine_values[NUM_VALUES];   // Array to store the received emotion values

SoftwareSerial Music_Serial(9, 8); // tx, rx pins for communication with DFPlayer Mini

// Create the Player object for controlling the music
DFRobotDFPlayerMini Music_player;

const unsigned long MUSICS = 30000;  // Duration for playing music (in milliseconds)

unsigned long last_played_music = 0;  // Tracks the time the last music was played

bool player_ready = false; // Flag to check if player is ready

// Setup function is called once to set up the system
void setup()
{
  delay(5000); // Wait for a while (to simulate hardware boot-up)
  
  // Initialize emotion values
  state_machine_values[0] = 500;
  state_machine_values[1] = 500;
  state_machine_values[2] = 500;
  state_machine_values[3] = 500;
  state_machine_values[4] = 500;

  // Initialize I2C communication for state machine
  Wire.begin(SLAVE_ADDRESS);     // Join the I2C bus as a slave with a specific address
  Wire.onReceive(receiveEvent);  // Register an event to handle incoming data

  // Initialize serial communication for debugging
  Serial.begin(9600);

  Music_Serial.begin(9600);  // Initialize serial for communication with DFPlayer Mini
  
  // Attempt to initialize DFPlayer Mini
  for (int i = 0; i < 5; i++) {
    if (Music_player.begin(Music_Serial)) {
      Serial.println("Music_player OK");

      // Set the music player volume (0-30)
      Music_player.volume(2); // 30 is very loud, so 2 is a reasonable volume
      player_ready = true;
      break;
    } else {
      Serial.println("Connecting to Music DFPlayer Mini failed! Retrying...");
      delay(500);  // Retry every 500ms
    }
  }
  if (!player_ready) {
    Serial.println("Music_Serial initialization failed after 5 attempts.");
  }
}

// Main loop function
void loop()
{
  doSomething(); // Call the function to trigger actions based on emotion values
  printStateMachineValues(); // Display the current emotion values
  delay(1000); // Delay of 1 second before repeating the loop
}

// Function that makes decisions based on emotion values
void doSomething()
{
  unsigned long currentTime = millis(); // Get the current time (since the program started)

  if (state_machine_values[0] < 300) {
    // If the first emotion value is less than 300, play song 2
    last_played_music = playSongs(Music_player, 2, MUSICS, last_played_music);
  }
  if (state_machine_values[0] > 700) {
    // If the first emotion value is greater than 700, play song 1
    last_played_music = playSongs(Music_player, 1, MUSICS, last_played_music);
  }
}

// Function to print emotion values in the serial monitor
void printStateMachineValues()
{
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

    // Display emotion based on the value
    if (state_machine_values[i] < 300) {
      Serial.print(negative_labels[i]);  // Negative emotion
    } else if (state_machine_values[i] <= 700) {
      Serial.print("Neutral");  // Neutral state
    } else {
      Serial.print(positive_labels[i]);  // Positive emotion
    }

    Serial.print(" (");
    Serial.print(state_machine_values[i]);
    Serial.println(")");
  }
}

// Function to play songs based on the given parameters
unsigned long playSongs(DFRobotDFPlayerMini &player, int music_number, const unsigned long reset_timer, unsigned long last_played)
{
  if (player_ready) {
    unsigned long now = millis();
    unsigned long dif = now - last_played; // Calculate the time difference since the last song

    if ((dif > reset_timer) || last_played == 0) { // If it's time to play a new song or if no song is playing
        player.play(music_number);  // Play the song (1 or 2)
        Serial.println("PLAY");
        return now;  // Update the time when the music was last played
    } else {
      return last_played;  // Return the last played time if it's too soon to play again
    }
  }
  return last_played;  // If player is not ready, return the last played time
}

// Function that is triggered when data is received from the state machine
void receiveEvent(int howMany)
{
  // Check if the number of bytes received is correct
  if (howMany == NUM_VALUES * 2)
  {
    // Read the bytes and store them in the emotion values array
    for (int i = 0; i < NUM_VALUES; ++i)
    {
      byte x1, x2;
      x1 = Wire.read();  // Read the upper byte of the emotion value
      x2 = Wire.read();  // Read the lower byte
      state_machine_values[i] = (int)x1 << 8 | (int)x2; // Combine the two bytes into a single value
    }
  }
}
