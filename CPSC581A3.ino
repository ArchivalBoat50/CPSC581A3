#include <Servo.h>

// Pin Definitions
const int servoPin = 9;
const int buzzerPin = 8;
const int trigPin = 10;
const int echoPin = 11;
const int fanPin = 5;
const int thermistorPin = A0;

// Servo
Servo doorServo;

// Variables
bool doorOpen = false;
float temperature = 0.0;
const float tempThreshold = 24.0; // Temperature threshold in Celsius

void setup() {
  Serial.begin(9600);

  // Servo setup
  doorServo.attach(servoPin);
  doorServo.write(0); // Start with door closed
  Serial.println("Door initialized to closed position.");

  // Buzzer setup
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);

  // Ultrasonic sensor setup
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Fan setup
  pinMode(fanPin, OUTPUT);
  digitalWrite(fanPin, LOW); // Ensure fan is off at startup
}

void loop() {
  // Check for commands from the web app
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command == "OPEN_DOOR") {
      Serial.println("Command received: OPEN_DOOR");
      openDoor();
    }
  }

  // Check Ultrasonic Sensor
  if (!doorOpen && detectObject()) {
    Serial.println("Object detected by ultrasonic sensor.");
    openDoor();
  }

  // Read Temperature and control fan
  temperature = readTemperature();
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");

  if (temperature > tempThreshold) {
    digitalWrite(fanPin, HIGH); // Turn on fan
    Serial.println("Heat detected. Fan turned ON, ");
  } else {
    digitalWrite(fanPin, LOW); // Turn off fan
    Serial.println("Room is cool. Fan turned OFF");
  }

  // Add a delay to avoid overwhelming the processor
  delay(500);
}

// Function to open the door
void openDoor() {
  if (!doorOpen) {
    Serial.println("Opening door...");
    doorServo.write(180); // Rotate servo to open position
    tone(buzzerPin, 1000, 500); // Sound buzzer at 1kHz for 500ms
    doorOpen = true;
    delay(5000); // Keep door open for 5 seconds (adjust as needed)
    closeDoor(); // Automatically close the door
  } else {
    Serial.println("Door is already open.");
  }
}

// Function to close the door
void closeDoor() {
  Serial.println("Closing door...");
  doorServo.write(0); // Rotate servo to closed position
  doorOpen = false;
}

// Function to detect object using Ultrasonic Sensor
bool detectObject() {
  long duration;
  float distance;

  // Send a 10us pulse to trigger pin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read the echo pin
  duration = pulseIn(echoPin, HIGH, 30000); // Timeout after 30ms

  if (duration == 0) {
    // No echo received (timeout)
    return false;
  }

  // Calculate distance in cm
  distance = (duration * 0.0343) / 2;

  Serial.print("Ultrasonic Sensor Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Check if object is within 30 cm
  if (distance > 0 && distance <= 10) {
    return true;
  } else {
    return false;
  }
}

// Function to read temperature from thermistor
float readTemperature() {
  int adcValue = analogRead(thermistorPin);
  float voltage = adcValue * (5.0 / 1023.0);
  float resistance = (5.0 / voltage - 1) * 10000; // Assuming a 10kΩ thermistor and 10kΩ resistor
  float temperatureC = 1 / (log(resistance / 10000) / 3950 + 1 / 298.15) - 273.15;
  return temperatureC;
}
