#include <SoftwareSerial.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <Fonts/FreeSansBoldOblique12pt7b.h>

// ---------------- Bluetooth Pins ----------------
#define BT_RX 0 // Connect to TX of HC-05
#define BT_TX 1  // Connect to RX of HC-05
SoftwareSerial BT(BT_RX, BT_TX);

// ---------------- Motor Driver Pins ----------------
#define ENA 3
#define IN1 4
#define IN2 5
#define ENB 6
#define IN3 7
#define IN4 12

char command;
int speedValue = 200;

// ---------------- Display Pins ----------------
#define TFT_CS   10
#define TFT_DC   9
#define TFT_RST  8

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// ---------------- Colors ----------------
#define BG_WHITE ST77XX_WHITE
#define BG_BLACK ST77XX_BLACK
#define EYE_WHITE ST77XX_WHITE
#define EYE_BLACK ST77XX_BLACK
#define BLUSH_COLOR 0xFDB8
#define SMILE_COLOR ST77XX_BLACK
#define TWINKLE ST77XX_YELLOW

// ---------------- Timers ----------------
unsigned long lastTwinkle = 0;
unsigned long lastBlink = 0;
bool eyesClosed = false;
unsigned long blinkStart = 0;
const int BLINK_DURATION = 200;
const unsigned long BLINK_INTERVAL = 1500;

// ---------------- Setup ----------------
void setup() {
  // Motor setup
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Bluetooth setup
  BT.begin(9600);
  Serial.begin(9600);
  Serial.println("Bluetooth Car + VIORA Display Ready");

  // TFT setup
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(1);
  showStartup();
  drawFace();
}

// ---------------- Loop ----------------
void loop() {
  handleBluetooth();
  handleDisplay();
}

// ====================================================
// ===============   BLUETOOTH CONTROL   ==============
// ====================================================
void handleBluetooth() {
  if (BT.available()) {
    command = BT.read();
    Serial.print("Command: ");
    Serial.println(command);

    if (command >= '0' && command <= '9') {
      speedValue = map(command - '0', 0, 9, 0, 255);
      Serial.print("Speed set to: ");
      Serial.println(speedValue);
    } else {
      switch (command) {
        case 'F': moveForward(); break;
        case 'B': moveBackward(); break;
        case 'L': turnLeft(); break;
        case 'R': turnRight(); break;
        case 'S': stopCar(); break;
        default: stopCar(); break;
      }
    }
  }
}

void moveForward() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  analogWrite(ENA, speedValue); analogWrite(ENB, speedValue);
}

void moveBackward() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
  analogWrite(ENA, speedValue); analogWrite(ENB, speedValue);
}

void turnLeft() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  analogWrite(ENA, speedValue); analogWrite(ENB, speedValue);
}

void turnRight() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
  analogWrite(ENA, speedValue); analogWrite(ENB, speedValue);
}

void stopCar() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
}

// ====================================================
// ===============     DISPLAY ANIMATION   ============
// ====================================================
void handleDisplay() {
  unsigned long currentMillis = millis();

  // Blink
  if (!eyesClosed && currentMillis - lastBlink > BLINK_INTERVAL) {
    closeEyes();
    eyesClosed = true;
    blinkStart = currentMillis;
  }
  if (eyesClosed && currentMillis - blinkStart >= BLINK_DURATION) {
    drawEyes();
    eyesClosed = false;
    lastBlink = currentMillis;
  }

  // Twinkle
  if (currentMillis - lastTwinkle > 4000) {
    twinkleEyes();
    lastTwinkle = currentMillis;
  }
}

// ---------------- Startup ----------------
void showStartup() {
  tft.fillScreen(BG_BLACK);
  tft.setFont(&FreeSansBoldOblique12pt7b);
  tft.setTextColor(ST77XX_WHITE);

  int16_t x1, y1;
  uint16_t w, h;
  tft.getTextBounds("VIORA", 0, 0, &x1, &y1, &w, &h);
  tft.setCursor((tft.width() - w) / 2, (tft.height() + h) / 2);
  tft.print("VIORA");

  delay(6000);
  tft.fillScreen(BG_WHITE);
}

// ---------------- Face ----------------
void drawFace() {
  tft.fillScreen(BG_WHITE);
  tft.fillCircle(35, 85, 7, BLUSH_COLOR);
  tft.fillCircle(125, 85, 7, BLUSH_COLOR);
  drawEyes();
  drawSmile();
}

// ---------------- Smile ----------------
void drawSmile() {
  int centerX = 80;
  int centerY = 105;
  for (int i = -16; i <= 16; i++) {
    int x = centerX + i;
    int y = centerY - ((i * i) / 60);
    tft.drawPixel(x, y, SMILE_COLOR);
  }
}

// ---------------- Eyes ----------------
void drawEyes() {
  tft.fillCircle(50, 60, 18, EYE_BLACK);
  tft.fillCircle(55, 55, 5, EYE_WHITE);
  tft.fillCircle(110, 60, 18, EYE_BLACK);
  tft.fillCircle(115, 55, 5, EYE_WHITE);
}

void closeEyes() {
  tft.fillCircle(50, 60, 18, BG_WHITE);
  tft.fillCircle(110, 60, 18, BG_WHITE);
  tft.fillCircle(35, 85, 7, BLUSH_COLOR);
  tft.fillCircle(125, 85, 7, BLUSH_COLOR);
  drawSmile();
}

void twinkleEyes() {
  tft.fillCircle(55, 55, 5, TWINKLE);
  tft.fillCircle(115, 55, 5, TWINKLE);
  delay(150);
  tft.fillCircle(55, 55, 5, EYE_WHITE);
  tft.fillCircle(115, 55, 5, EYE_WHITE);
}