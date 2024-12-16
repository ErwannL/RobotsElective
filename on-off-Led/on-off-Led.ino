
const int buttonPin = 5;  // Pin for the button
int buttonPressCount = 0; // Counter for button presses
int lol = 0;

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(9600);
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(buttonPin, INPUT);   // Set the button pin as input
}

void loop() {
  static bool lastButtonState = LOW; // Variable to store the previous button state

  if (digitalRead(buttonPin) == HIGH && lastButtonState == LOW) { 
    if (lol == 5) {
      // Increment the counter when the button is pressed
      buttonPressCount++;
      Serial.print("Button pressed ");
      Serial.print(buttonPressCount);
      Serial.println(" times.");
    } else {
      lol++;
    }
  }

  // Update the LED based on button state
  if (digitalRead(buttonPin) == HIGH) { 
    digitalWrite(LED_BUILTIN, HIGH); // Turn the LED on
  } else {
    digitalWrite(LED_BUILTIN, LOW);  // Turn the LED off
  }

  // Save the current button state
  lastButtonState = digitalRead(buttonPin);
}
