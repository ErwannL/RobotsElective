
#include <Wire.h>
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

//the i2c address of your Arduino
#define SLAVE_ADDRESS 0x01  // change this to 0x01, 0x02, 0x03, 0x04 or 0x05, depending on the values chosen by your teammates

//used for communicating the emotion values
#define NUM_VALUES 5                    // number of values to send
int state_machine_values[NUM_VALUES];   // array to store the received emotion values

SoftwareSerial Songs_Serial(9, 8); //tx, rx

// Create the Player object
DFRobotDFPlayerMini Songs_player;

const unsigned long SONGS = 15000;

unsigned long last_played_song = 0;

//function setup is always needed and is called once to setup your arduino
void setup()
{

  state_machine_values[0] = 800;
  state_machine_values[1] = 500;
  state_machine_values[2] = 500;
  state_machine_values[3] = 500;
  state_machine_values[4] = 500;

  //needed for i2c communication between state machine and your arduino
  Wire.begin(SLAVE_ADDRESS);     // join the I2C bus as a slave
  Wire.onReceive(receiveEvent);  // register event to handle incoming data

  //used for showing data in the serial monitor
  Serial.begin(9600);  // start serial communication

  Songs_Serial.begin(9600);
  if (Songs_player.begin(Songs_Serial)) {
    Serial.println("Songs_player OK");

    // Set volume to maximum (0 to 30).
    Songs_player.volume(2); //30 is very loud
  } else {
    Serial.println("Connecting to Songs DFPlayer Mini failed!");
  }

}

void loop()
{
  doSomething(); //example function
  printStateMachineValues(); //show the values that the state machine communicated
  delay(1000); // an arbitrary delay, can be changed to whatever you need
}

//Yes, this is written inefficient, but very easy to understand ;)
//It sets each of the 5 values for changing the emotions by randomly assigning a 0, 1 or -1 to each
void doSomething()
{
  // Current time for tracking
  unsigned long currentTime = millis();

  if (state_machine_values[0] > 700) {
    int randomLaugh = random(1, 6); // Get a random between 1 and 5
    last_played_song = playSongs(Songs_player, randomLaugh, SONGS, last_played_song);
  }

  if ((state_machine_values[2] < 300) || (state_machine_values[4] < 300)) {
    last_played_song = playSongs(Songs_player, 6, SONGS, last_played_song);
  }

  if ((state_machine_values[2] < 200) || (state_machine_values[4] < 200)) {
    last_played_song = playSongs(Songs_player, 7, SONGS, last_played_song);
  }

  if ((state_machine_values[2] < 100) || (state_machine_values[4] < 100)) {
    last_played_song = playSongs(Songs_player, 8, SONGS, last_played_song);
  }
}

//example how to print the values from the state machine
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

unsigned long playSongs(DFRobotDFPlayerMini &player, int music_number, const unsigned long reset_timer, unsigned long last_played) {

  unsigned long now = millis();
  unsigned long dif = now - last_played;
  if ((dif > reset_timer) || last_played == 0) { // Si aucune musique ne joue, démarre une nouvelle
      Serial.println(player.readState()); //read mp3 state
      player.play(music_number);
      Serial.println("PLAY");
      return now;
  } else {
    return last_played;
  }
}

//###############
//below functions are for sending and receiving data to the state machine, they should not be changed
//###############

// function that executes whenever data is received from the state machine
void receiveEvent(int howMany)
{
  // check if the number of bytes received matches the expected number
  if (howMany == NUM_VALUES * 2)
  {
    // read the bytes and store them in the values array
    for (int i = 0; i < NUM_VALUES; ++i)
    {
      byte x1, x2;
      x1 = Wire.read();  //x1 holds upper byte of received numPWMChannels
      x2 = Wire.read();  //x2 holds lower byte of received numPWMChannels
      state_machine_values[i] = (int)x1 << 8 | (int)x2;
    }
  }
}