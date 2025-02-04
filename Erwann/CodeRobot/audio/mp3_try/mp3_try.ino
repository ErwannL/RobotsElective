#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

SoftwareSerial Songs_Serial(9, 8); //tx, rx

// Create the Player object
DFRobotDFPlayerMini Songs_player;

int music = 1;

void setup() 
{
  Serial.begin(9600);

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
  Serial.println("Playing");
  Songs_player.play(music);
  Serial.println("waiting");
  delay(10000);
  Serial.println("adding");
  music += 1;
  if (music == 6) {
    music = 1;
  }
}
