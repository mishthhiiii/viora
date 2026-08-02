#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <Fonts/FreeSansBoldOblique12pt7b.h> // bold + italic font

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
const int BLINK_DURATION = 200;         // blink duration in ms
const unsigned long BLINK_INTERVAL = 1500; // blink every 1.5 seconds

void setup() {
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(1);

  showStartup();
  drawFace(); // draw initial face
}

void loop() {
  unsigned long currentMillis = millis();

  // Blink every 1.5 seconds
  if (!eyesClosed && currentMillis - lastBlink > BLINK_INTERVAL) {
    closeEyes();        // instant close
    eyesClosed = true;
    blinkStart = currentMillis;
  }

  // Reopen eyes after blink duration
  if (eyesClosed && currentMillis - blinkStart >= BLINK_DURATION) {
    drawEyes();         // instant open
    eyesClosed = false;
    lastBlink = currentMillis;
  }

  // Twinkle every 4 seconds
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

  // Center the text
  int16_t x1, y1;
  uint16_t w, h;
  tft.getTextBounds("VIORA", 0, 0, &x1, &y1, &w, &h);
  tft.setCursor((tft.width() - w) / 2, (tft.height() + h) / 2);

  tft.print("VIORA"); // display startup text

  delay(6000);         // show for 6 seconds
  tft.fillScreen(BG_WHITE); // clear screen for face
}

// ---------------- Face ----------------
void drawFace() {
  tft.fillScreen(BG_WHITE);

  // blush slightly upward
  tft.fillCircle(35, 85, 7, BLUSH_COLOR);
  tft.fillCircle(125, 85, 7, BLUSH_COLOR);

  drawEyes();
  drawSmile();
}

// ---------------- Happy Smile ----------------
void drawSmile() {
  int centerX = 80;
  int centerY = 105;

  for (int i = -16; i <= 16; i++) {
    int x = centerX + i;
    int y = centerY - ((i * i) / 60);
    tft.drawPixel(x, y, SMILE_COLOR);
  }
}

// ---------------- Eyes (Instant Open) ----------------
void drawEyes() {
  // left eye
  tft.fillCircle(50, 60, 18, EYE_BLACK);
  tft.fillCircle(55, 55, 5, EYE_WHITE);

  // right eye
  tft.fillCircle(110, 60, 18, EYE_BLACK);
  tft.fillCircle(115, 55, 5, EYE_WHITE);
}

// ---------------- Close Eyes (Instant) ----------------
void closeEyes() {
  // Cover the black eyes instantly
  tft.fillCircle(50, 60, 18, BG_WHITE);
  tft.fillCircle(110, 60, 18, BG_WHITE);

  // Redraw blush and smile
  tft.fillCircle(35, 85, 7, BLUSH_COLOR);
  tft.fillCircle(125, 85, 7, BLUSH_COLOR);
  drawSmile();
}

// ---------------- Twinkle Eyes ----------------
void twinkleEyes() {
  // draw sparkle
  tft.fillCircle(55, 55, 5, TWINKLE);
  tft.fillCircle(115, 55, 5, TWINKLE);

  delay(150); // small pause for sparkle
  tft.fillCircle(55, 55, 5, EYE_WHITE);
  tft.fillCircle(115, 55, 5, EYE_WHITE);
}
