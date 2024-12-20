// Sensors and peripherals
const int sensorPin = 6; // Pin connected to the touch sensor
const int buzzerPin = 8; // Pin connected to the buzzer

// Variables for tracking state changes
byte change_in_happy = 0;     // Tracks changes in happiness
byte change_in_energy = 0;    // Tracks changes in energy
byte change_in_friendly = 0;  // Tracks changes in friendliness
byte change_in_assertive = 0; // Tracks changes in assertiveness
byte change_in_delight = 0;   // Tracks changes in delight

// Function to activate the buzzer with a fixed frequency and duration
void buzz() {
  tone(buzzerPin, 1000, 500); // Emit a 1000 Hz sound for 500 ms
}

// Function to adjust state changes directly based on inputs
void adjustEmotionChanges(bool sensorInput) {
  if (sensorInput) {
    change_in_delight = -1; // Decrease delight if sensor is active
    buzz(); // Trigger buzzer for combined low values
  } else {
    // Default adjustments when energy and assertiveness are sufficient
    change_in_assertive = 1;
    change_in_friendly = 1;
  }
}

void setup() {
  pinMode(sensorPin, INPUT);  // Configure sensor pin as input
  pinMode(buzzerPin, OUTPUT); // Configure buzzer pin as output
}

void loop() {
  // Read the state of the touch sensor
  bool sensorInput = digitalRead(sensorPin); // TRUE if touched, FALSE otherwise

  // Adjust emotional state changes based on inputs
  adjustEmotionChanges(sensorInput);

  delay(100); // Wait before the next loop iteration
}
