#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite arrow = TFT_eSprite(&tft);

#define SCREEN_W 240
#define SCREEN_H 320
#define ARROW_W  50
#define ARROW_H  66

void drawArrow(int angle) {
    arrow.fillSprite(TFT_BLACK);
    // Shaft
    arrow.fillRect(22, 28, 6, 36, TFT_WHITE);
    // Head
    arrow.fillTriangle(25, 2, 4, 30, 46, 30, TFT_WHITE);
    // pushRotated uses tft pivot as screen position
    arrow.pushRotated(angle);
}

void setup() {
    Serial.begin(115200);
    delay(500);
    Serial.println("[START]");

    tft.init();
    tft.setRotation(0);
    tft.invertDisplay(true);
    Serial.println("[OK] init");

    // --- Color fills ---
    Serial.println("[TEST] RED");
    tft.fillScreen(TFT_RED);
    delay(1000);

    Serial.println("[TEST] GREEN");
    tft.fillScreen(TFT_GREEN);
    delay(1000);

    Serial.println("[TEST] BLUE");
    tft.fillScreen(TFT_BLUE);
    delay(1000);

    // --- Text ---
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);
    tft.setCursor(10, 10);
    tft.print("TFT_eSPI OK");
    Serial.println("[OK] text");
    delay(1500);

    // --- Arrow sprite ---
    arrow.createSprite(ARROW_W, ARROW_H);
    arrow.setPivot(ARROW_W / 2, ARROW_H / 2);  // Rotate around sprite center

    // Place arrow center in the middle of the screen
    tft.setPivot(SCREEN_W / 2, SCREEN_H / 2);

    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.setTextSize(2);

    Serial.println("[TEST] arrow sweep");
    int angles[]         = {0, 45, 90, 135, 180, -135, -90, -45, 0};
    const char* labels[] = {"UP", "NE", "RIGHT", "SE", "DOWN", "SW", "LEFT", "NW", "UP"};

    for (int i = 0; i < 9; i++) {
        // Print label centered at top
        tft.fillRect(0, 0, SCREEN_W, 20, TFT_BLACK);
        tft.setCursor((SCREEN_W - strlen(labels[i]) * 12) / 2, 4);
        tft.print(labels[i]);
        drawArrow(angles[i]);
        Serial.print("[ARROW] ");
        Serial.println(labels[i]);
        delay(700);
    }

    Serial.println("[DONE]");
}

void loop() {}
