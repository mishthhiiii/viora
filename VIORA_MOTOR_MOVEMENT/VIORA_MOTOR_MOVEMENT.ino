#include <SoftwareSerial.h>

// ----- Bluetooth Pins -----
#define BT_RX 2  // Connect to TX of HC-05
#define BT_TX 3  // Connect to RX of HC-05
SoftwareSerial BT(BT_RX, BT_TX);

// ----- Motor Driver Pins -----
#define ENA 5   // Enable pin for Left motor
#define IN1 8
#define IN2 9
#define ENB 6   // Enable pin for Right motor
#define IN3 10
#define IN4 11

char command;
int speedValue = 200;  // Default speed (range 0–255)

void setup() {
  // Motor pins
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Bluetooth setup
  BT.begin(9600);
  Serial.begin(9600);
  Serial.println("Bluetooth Car Ready — waiting for commands...");
}

void loop() {
  if (BT.available()) {
    command = BT.read();
    Serial.print("Command: ");
    Serial.println(command);

    if (command >= '0' && command <= '9') {
      // Map speed 0–9 to PWM 0–255
      speedValue = map(command - '0', 0, 9, 0, 255);
      Serial.print("Speed set to: ");
      Serial.println(speedValue);
    } 
    else {
      switch (command) {
        case 'F':
          moveForward();
          break;
        case 'B':
          moveBackward();
          break;
        case 'L':
          turnLeft();
          break;
        case 'R':
          turnRight();
          break;
        case 'S':
          stopCar();
          break;
        default:
          stopCar();
          break;
      }
    }
  }
}

// ----- Motor Functions -----
void moveForward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, speedValue);
  analogWrite(ENB, speedValue);
}

void moveBackward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, speedValue);
  analogWrite(ENB, speedValue);
}

void turnLeft() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, speedValue);
  analogWrite(ENB, speedValue);
}

void turnRight() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, speedValue);
  analogWrite(ENB, speedValue);
}

void stopCar() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}