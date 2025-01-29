

#include <Wire.h>
                  // Pour stocker les valeurs lues
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"


SoftwareSerial Songs_Serial(11, 10); //tx, rx
// defines variables
long duration;
int distance;
const int trigPin = 13;
const int echoPin = 12;
#define SLAVE_ADDRESS 0x02 
#define ADXL345_Adresse 0x53 // adresse de l'ADXL345
#define POWER_CTL 0x2D // registre Power Control
#define DATA_FORMAT 0x31 // registre Data Format
#define DATAX0 0x32 // bit de poids faible axe X
#define DATAX1 0x33 // bit de poids fort axe X
#define DATAY0 0x34 // bit de poids faible axe Y
#define DATAY1 0x35 // bit de poids fort axe Y
#define DATAZ0 0x36 // bit de poids faible axe Z
#define DATAZ1 0x37 // bit de poids fort axe Z
#define ADXL345_Precision2G 0x00
#define ADXL345_Precision4G 0x01
#define ADXL345_Precision8G 0x02
#define ADXL345_Precision16G 0x03
#define ADXL345_ModeMesure 0x08
byte buffer[6]; // stockage des données du module
int i = 0;
int composante_X;
int composante_Y;
int composante_Z;
int ref_Z;
int rnd;
int Flag_scream=0;
int Flag_Dance=0;
int Diff;


#define NUM_PINS 4                      // number of analog pins to read
const int PINS[] = { A0, A1, A2, A3 };  // array of analog pins
int analog_values[NUM_PINS];                   // array to store the analog values


//used for communicating the emotion values
#define NUM_VALUES 5                    // number of values to send
int state_machine_values[NUM_VALUES];   // array to store the received emotion values


//These are the values you can change by giving them te value -1, 1, or 0 (no change)
byte change_in_happy = 0;               //the statemachine value for happy ranges from 0 (sad) to 1000 (happy)
byte change_in_energy = 0;              //the statemachine value for energy ranges from 0 (lethargic) to 1000 (energetic)
byte change_in_friendly = 0;            //the statemachine value for friendly ranges from 0 (hostile) to 1000 (friendly)
byte change_in_assertive = 0;           //the statemachine value for assertive ranges from 0 (shy) to 1000 (assertive)
byte change_in_delight = 0;             //the statemachine value for delight ranges from 0 (disgust) to 1000 (delight)
DFRobotDFPlayerMini Songs_player;






void setup() {
   pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input

  Wire.begin(SLAVE_ADDRESS);     // join the I2C bus as a slave
  Wire.onRequest(requestEvent);  // register the request event handler
  Wire.onReceive(receiveEvent);  // register event to handle incoming data



  Serial.begin(9600); // Starts the serial communication


  // Configurer les broches 2 à 9 comme sorties
  for (int pin = 2; pin <= 9; pin++) {
    pinMode(pin, OUTPUT);
  }
   digitalWrite(3,HIGH);
  digitalWrite(4,HIGH);
  digitalWrite(6,HIGH);
  digitalWrite(9,HIGH);
  Wire.begin (); // initialisation de la liaison I2C
Wire.beginTransmission (ADXL345_Adresse); // configuration du module
Wire.write (DATA_FORMAT);
Wire.write (ADXL345_Precision4G);
Wire.endTransmission ();
Wire.beginTransmission (ADXL345_Adresse);
Wire.write (POWER_CTL);
Wire.write (ADXL345_ModeMesure);
Wire.endTransmission ();
Wire.beginTransmission (ADXL345_Adresse);
Wire.write(DATAX0);
Wire.endTransmission ();
Wire.beginTransmission (ADXL345_Adresse);
Wire.requestFrom(ADXL345_Adresse, 6); // récupération des 6 composantes
i=0;
while(Wire.available())
{
buffer[i] = Wire.read();
i++;
}
Wire.endTransmission();
composante_X=(buffer[1] << 8) | buffer[0]; // élaboration des 3 composantes
composante_Y=(buffer[3] << 8) | buffer[2];
composante_Z=(buffer[5] << 8) | buffer[4];


ref_Z=composante_Z;



Songs_Serial.begin(9600);
  if (Songs_player.begin(Songs_Serial)) {
    Serial.println("Songs_player OK");


    // Set volume to maximum (0 to 30).
    Songs_player.volume(30); //30 is very loud
  } else {
    Serial.println("Connecting to Songs DFPlayer Mini failed!");
  }


 
  }
 




void loop() {
  
   // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2;
  // Prints the distance on the Serial Monitor
  // Serial.print("Distance: ");
  // Serial.println(distance);
  if (state_machine_values[3]<300){
    Serial.println("entrer");
    digitalWrite(3,LOW);
    digitalWrite(4,LOW);
    digitalWrite(6,LOW);
    digitalWrite(9,LOW);
    if (state_machine_values[3]<200){
      if (Flag_scream==0){
         Serial.println("Playing");
    Songs_player.play(1);
    Flag_scream=1;
      }
    }
    else{
      Flag_scream=0;
    }}
else{

  if (distance<=50){
  digitalWrite(3,LOW);
  digitalWrite(4,LOW);
  digitalWrite(6,LOW);
  digitalWrite(9,LOW);
  }
  else{
    digitalWrite(3,HIGH);
  digitalWrite(4,HIGH);
  digitalWrite(6,HIGH);
  digitalWrite(9,HIGH);
  }
  if (state_machine_values[0]>700){
    Dance();
  }
  }
  Wire.beginTransmission (ADXL345_Adresse);
Wire.write(DATAX0);
Wire.endTransmission ();
Wire.beginTransmission (ADXL345_Adresse);
Wire.requestFrom(ADXL345_Adresse, 6); // récupération des 6 composantes
i=0;
while(Wire.available())
{
buffer[i] = Wire.read();
i++;
}
Wire.endTransmission();
composante_X=(buffer[1] << 8) | buffer[0]; // élaboration des 3 composantes
composante_Y=(buffer[3] << 8) | buffer[2];
composante_Z=(buffer[5] << 8) | buffer[4];
Diff=abs(composante_Z-ref_Z);
if (Diff>20){
  Serial.println("entrer");
  rnd = random(-100, 100);
  if (rnd<0){
    change_in_assertive=-1;
    Serial.println("request sent");
    requestEvent();
    change_in_assertive=0;

  }
}}




void printStateMachineValues()
{
  Serial.print("State Machine Values: ");
  for (int i = 0; i < NUM_VALUES; i++)
  {
    Serial.print(state_machine_values[i]);
    Serial.print(" ");
  }
  Serial.println();
}


void requestEvent()
{
  byte change[NUM_VALUES] = { change_in_happy, change_in_energy, change_in_friendly, change_in_assertive, change_in_delight };                     // array to store the change of each emotion
  Wire.write(change, NUM_VALUES);  // send the bytes to the master
}


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

void Dance(){
  digitalWrite(3,LOW);
  digitalWrite(4,LOW);
  digitalWrite(6,LOW);
  digitalWrite(9,LOW);
  digitalWrite(2,LOW);
  digitalWrite(5,LOW);
  digitalWrite(7,LOW);
  digitalWrite(8,LOW);
if(Flag_Dance==0){
  digitalWrite(2,HIGH);
  digitalWrite(8,HIGH);
  
}
else {
  digitalWrite(5,HIGH);
  digitalWrite(7,HIGH);
}
Flag_Dance=(Flag_Dance+1)%2;
delay (500);
}
