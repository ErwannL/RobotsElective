#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

SoftwareSerial Songs_Serial(9, 8); //tx, rx
SoftwareSerial Laugh_Serial(12, 11); //tx, rx

// Create the Player object
DFRobotDFPlayerMini Songs_player;
DFRobotDFPlayerMini Laugh_player;

bool hasPlayed = false;

void setup() 
{
  Serial.begin(19200);

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
  if (!hasPlayed) {
    Serial.println("Playing");
    Songs_player.play(1);
    Laugh_player.play(1);
    hasPlayed = true; // Marquer que les chansons ont été jouées
  }

  // Une fois que la chanson a été jouée, on peut soit faire autre chose, soit laisser le loop vide
}
