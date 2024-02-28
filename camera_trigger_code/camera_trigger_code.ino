// Arduino code written by ENGN0350 Art Fluid Engineering Group 5 for lab 1 February 2024
// this program waits for a button to be pressed, runs a pump for a short period of time, turns on a laser, waits for a phototransistor to detect a falling droplet, waits an adjustable amount of time, then triggers a camera and flash to take a photo
// this program was edited quickly during lab and ended up a bit messy.
// const int pumpPulse = 19; // milliseconds of running pump
const float shutterDelayScalar = 600.0;
const boolean twoDrop = false;
const float twoDropDelayScalar = 70.0;

const byte sensorPin = A5; // photodiode to detect droplet
const byte laserPin = 9; // pin that powers laser
const byte cameraPin = 12; // pin that triggers camera
const byte buttonPin = 3; // pin connected to start button
const byte pumpPin = 6;  // pin connected to pump
const byte knobPin = A0; // pin connected to potentiometer
const byte knobTwoPin = A1; // pin connected to potentiometer
const byte flashPin = 8; // connected to flash
const byte pairedFlashPin = 11; // goes off when flashPin does (after shutter delay) incase you want to do two things

boolean wasButtonPressed = false; // used to detect button press
boolean buttonPressed = false;
boolean sequenceStarted = false; // goes true when button pressed
unsigned long sequenceStartMillis = 0; // value of millis() when sequence started, for timing pump pulse

float knobVal = 0; // 0.0-1.0, can be used to adjust things
float knobTwoVal = 0; // 0.0-1.0, can be used to adjust things

void setup() {
  pinMode(sensorPin, INPUT_PULLUP); // activate pullup resistor to power photo transistor
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(knobPin, INPUT);
  pinMode(knobTwoPin, INPUT);
  pinMode(laserPin, OUTPUT);
  pinMode(cameraPin, OUTPUT);
  pinMode(pumpPin, OUTPUT);
  pinMode(flashPin, OUTPUT);
  pinMode(pairedFlashPin, OUTPUT);
  laserOn(true); // start with laser on, so we can align the laser
  Serial.begin(115200); // for debugging
  delay(50);
}

void loop() {
  if (!sequenceStarted) {
    knobVal = (1023 - analogRead(knobPin)) / 1023.0; // 0.0-1.0, we can use this to adjust things
    Serial.print("pic delay (s): ");
    Serial.print(knobVal * shutterDelayScalar / 1000.0, 4);
    //    if (twoDrop) {
    knobTwoVal = (analogRead(knobTwoPin)) / 1023.0; // 0.0-1.0, we can use this to adjust things
    Serial.print(", mump time (s): ");
    Serial.println(knobTwoVal * twoDropDelayScalar / 1000.0, 4);
    //    }
  }
  buttonPressed = digitalRead(buttonPin) == LOW;
  if (buttonPressed && !wasButtonPressed) { // button just pressed
    if (!sequenceStarted) {
      laserOn(true);
      delay(50); // let laser turn on
      sequenceStartMillis = millis();
      sequenceStarted = true;
    }
  }
  wasButtonPressed = buttonPressed; // remember last button state so we can detect presses

  if (sequenceStarted) { // pulse pump
    if ((millis() - sequenceStartMillis) < (twoDropDelayScalar * knobTwoVal)) { // pump pulse
      // run pump for time or until canceled by droplet detection
      runPump(true);
      //    } else if (twoDrop && (millis() - sequenceStartMillis) > twoDropDelayScalar * knobTwoVal && (millis() - sequenceStartMillis) < (pumpPulse + twoDropDelayScalar * knobTwoVal)) {
      //      runPump(true);
    } else {
      runPump(false);
    }
  }
  //&&(!twoDrop||(millis() - sequenceStartMillis) > pumpPulse+twoDropDelayScalar * knobTwoVal)
  if (sequenceStarted && (digitalRead(sensorPin) == HIGH)) { // start photo sequence, droplet detected
    laserOn(false); // turn off laser
    runPump(false);
    takeAPhoto();
    sequenceStarted = false;
  }
}

// take a photo
void takeAPhoto() {
  digitalWrite(cameraPin, HIGH);
  delay(shutterDelayScalar * knobVal); // add additonal lag here
  digitalWrite(flashPin, HIGH);
  digitalWrite(pairedFlashPin, HIGH);
  delay(100);                       // flash
  digitalWrite(flashPin, LOW);
  digitalWrite(pairedFlashPin, LOW);
  delay(200); // wait for camera
  digitalWrite(cameraPin, LOW);
  delay(100); //wait for camera
}

void runPump(boolean on) {
  analogWrite(pumpPin, on ? 190 : 0);
}

void laserOn(boolean on) {
  if (on) {
    digitalWrite(laserPin, HIGH);
  } else {
    digitalWrite(laserPin, LOW);
  }
}
