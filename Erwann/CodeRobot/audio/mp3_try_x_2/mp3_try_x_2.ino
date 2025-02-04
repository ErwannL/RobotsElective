#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

SoftwareSerial Songs_Serial(12, 11); // TX, RX pour le premier lecteur
SoftwareSerial Songs_Serial_2(9, 8); // TX, RX pour le second lecteur

// Création des objets DFPlayer
DFRobotDFPlayerMini Songs_player;
DFRobotDFPlayerMini Songs_player_2;

// Variables pour suivre si les lecteurs sont fonctionnels
bool player1_ready = false;
bool player2_ready = false;

int music = 1;
int music_2 = 1;

unsigned long now;
unsigned long last = 0;
unsigned long waiting_time = 0; // 30 secondes

void setup() 
{
  Serial.begin(9600);

  Songs_Serial.begin(9600);
  // Tentatives d'initialisation pour le premier lecteur
  for (int i = 0; i < 5; i++) {
    if (Songs_player.begin(Songs_Serial)) {
      Serial.println("Songs_player OK");
      Songs_player.volume(3); // Réglage du volume (0 à 30)
      player1_ready = true;
      break;
    } else {
      Serial.println("Connecting to Songs DFPlayer Mini failed! Retrying...");
      delay(500);
    }
  }

  if (!player1_ready) {
    Serial.println("Songs_player initialization failed after 5 attempts.");
  }

  Songs_Serial_2.begin(9600);
  // Tentatives d'initialisation pour le second lecteur
  for (int i = 0; i < 5; i++) {
    if (Songs_player_2.begin(Songs_Serial_2)) {
      Serial.println("Songs_player_2 OK");
      Songs_player_2.volume(3);
      player2_ready = true;
      break;
    } else {
      Serial.println("Connecting to Songs 2 DFPlayer Mini failed! Retrying...");
      delay(500);
    }
  }

  if (!player2_ready) {
    Serial.println("Songs_player_2 initialization failed after 5 attempts.");
  }
}

void loop() 
{
  now = millis();
  if (now - last > waiting_time) {
    Serial.println("Playing");

    if (player1_ready) {
      Songs_Serial.listen();
      delay(100); // Petite pause pour éviter les conflits
      Songs_player.play(music);
      Serial.println("Player 1 playing");
      delay(1000); // Laisser le temps au premier player de répondre
    } else {
      Serial.println("Player 1 skipped due to initialization failure");
    }

    if (player2_ready) {
      Songs_Serial_2.listen();
      delay(100); // Petite pause pour éviter les conflits
      Songs_player_2.play(music_2);
      Serial.println("Player 2 playing");
    } else {
      Serial.println("Player 2 skipped due to initialization failure");
    }

    Serial.println("waiting...");

    // Incrémentation des musiques jouées
    music += 1;
    music_2 += 1;

    if (music > 5) {
      music = 1;
    }
    if (music_2 > 2) {
      music_2 = 1;
    }

    last = millis();
    waiting_time = 300000;
  } else {
    Serial.print("~");
    Serial.print((waiting_time - (now - last)) / 1000);
    Serial.println(" Secondes");
    delay(waiting_time / 100);
  }
}
