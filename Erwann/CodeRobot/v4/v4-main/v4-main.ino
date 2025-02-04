
#include <Wire.h>
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

//the i2c address of your Arduino
#define SLAVE_ADDRESS 0x01  // change this to 0x01, 0x02, 0x03, 0x04 or 0x05, depending on the values chosen by your teammates

//used for communicating the emotion values
#define NUM_VALUES 5                    // number of values to send
int state_machine_values[NUM_VALUES];   // array to store the received emotion values

//These are the values you can change by giving them te value -1, 1, or 0 (no change)
byte change_in_happy = 0;               //the statemachine value for happy ranges from 0 (sad) to 1000 (happy) 
byte change_in_energy = 0;              //the statemachine value for energy ranges from 0 (lethargic) to 1000 (energetic)
byte change_in_friendly = 0;            //the statemachine value for friendly ranges from 0 (hostile) to 1000 (friendly)
byte change_in_assertive = 0;           //the statemachine value for assertive ranges from 0 (shy) to 1000 (assertive)
byte change_in_delight = 0;             //the statemachine value for delight ranges from 0 (disgust) to 1000 (delight)













// Pin definitions
#define TOUCH_SENSOR_TOP 2
#define TOUCH_SENSOR_DOWN 3
#define TOUCH_SENSOR_RIGHT 4
#define TOUCH_SENSOR_LEFT 5
#define TOUCH_SENSOR_CENTER 6

SoftwareSerial Laugh_Serial(12, 11); //tx, rx

// Create the Player object
DFRobotDFPlayerMini Laugh_player;

// Threshold and time window for tickling
const int TICKLE_THRESHOLD = 5; // Minimum combined activations for a "tickle"
const unsigned long TICKLE_TIME = 500; // Time window in milliseconds
const unsigned long NO_TICKLE_TIME = 5000; // Time window in milliseconds
const unsigned long MUSICS = 30000;

unsigned long last_played_music = 0;

// Variables to track activations and time
int totalTouches = 0; // Global counter for combined activations
unsigned long lastTickleTime = 0;

// Sensor order: TOP, DOWN, RIGHT, LEFT, CENTER
int sensorPins[5] = {TOUCH_SENSOR_TOP, TOUCH_SENSOR_DOWN, TOUCH_SENSOR_RIGHT, TOUCH_SENSOR_LEFT, TOUCH_SENSOR_CENTER};

//function setup is always needed and is called once to setup your arduino
void setup()
{


  state_machine_values[0] = 500;
  state_machine_values[1] = 500;
  state_machine_values[2] = 500;
  state_machine_values[3] = 500;
  state_machine_values[4] = 500;






  //needed for i2c communication between state machine and your arduino
  Wire.begin(SLAVE_ADDRESS);     // join the I2C bus as a slave
  Wire.onRequest(requestEvent);  // register the request event handler
  Wire.onReceive(receiveEvent);  // register event to handle incoming data

  //used for showing data in the serial monitor
  Serial.begin(9600);  // start serial communication

  // Set sensor pins as inputs
  for (int i = 0; i < 5; i++) {
    pinMode(sensorPins[i], INPUT);
  }

  Laugh_Serial.begin(9600);
  if (Laugh_player.begin(Laugh_Serial)) {
    Serial.println("Laugh_player OK");

    // Set volume to maximum (0 to 30).
    Laugh_player.volume(2); //30 is very loud
  } else {
    Serial.println("Connecting to Laugh DFPlayer Mini failed!");
  }


  lastTickleTime = millis();
}

//function loop is always needed and is called continuously
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

  // Check each sensor and count activations
  for (int i = 0; i < 5; i++) {
    if (digitalRead(sensorPins[i]) == HIGH) {
      totalTouches++;
    }
  }

  // Current time for tracking
  unsigned long currentTime = millis();

  // If time window has passed, evaluate and reset
  if (currentTime - lastTickleTime >= TICKLE_TIME) {
    if (totalTouches >= TICKLE_THRESHOLD) {
      lastTickleTime = currentTime;
      totalTouches = 0;
      if (state_machine_values[4] < 300) {
        change_in_friendly = changeState(change_in_friendly, -1);
      } else {
        change_in_friendly = changeState(change_in_friendly, 1);
        change_in_happy = changeState(change_in_happy, 1);
      }
    } else {
      Serial.print("still ");
      Serial.print(TICKLE_THRESHOLD - totalTouches);
      Serial.println(" tickle before detected");
    }
  }

  sendToStateMachine();

  if (state_machine_values[0] < 300) {
    last_played_music = playSongs(Laugh_player, 2, MUSICS, last_played_music);
  }
  if (state_machine_values[0] > 700) {
    last_played_music = playSongs(Laugh_player, 1, MUSICS, last_played_music);
  }

  currentTime = millis();
  // If time window has passed, evaluate and reset
  if (currentTime - lastTickleTime >= NO_TICKLE_TIME) {
    change_in_happy = changeState(change_in_happy, -1);
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

void sendToStateMachine()
{
  requestEvent();




  Serial.print(change_in_happy);
  Serial.print(" ");
  Serial.print(change_in_energy);
  Serial.print(" ");
  Serial.print(change_in_friendly);
  Serial.print(" ");
  Serial.print(change_in_assertive);
  Serial.print(" ");
  Serial.println(change_in_delight);

  state_machine_values[0] += (change_in_happy == 1) ? 50 : (change_in_happy == 255) ? -50 : 0;
  state_machine_values[1] += (change_in_energy == 1) ? 50 : (change_in_energy == 255) ? -50 : 0;
  state_machine_values[2] += (change_in_friendly == 1) ? 50 : (change_in_friendly == 255) ? -50 : 0;
  state_machine_values[3] += (change_in_assertive == 1) ? 50 : (change_in_assertive == 255) ? -50 : 0;
  state_machine_values[4] += (change_in_delight == 1) ? 50 : (change_in_delight == 255) ? -50 : 0;

  for (int i = 0; i < NUM_VALUES; i++) {
    if (state_machine_values[i] < 0) {
      state_machine_values[i] = 0;
    } else if (state_machine_values[i] > 1000) {
      state_machine_values[i] = 1000;
    }
  }
}




























//###############
//below functions are for sending and receiving data to the state machine, they should not be changed
//###############

//The response for the request made by the statemachine to receive change values
void requestEvent()
{
  byte change[NUM_VALUES] = { change_in_happy, change_in_energy, change_in_friendly, change_in_assertive, change_in_delight };                     // array to store the change of each emotion
  Wire.write(change, NUM_VALUES);  // send the bytes to the master
}

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
