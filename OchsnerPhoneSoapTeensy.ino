#include <FastLED.h>

FASTLED_USING_NAMESPACE

#define DATA_PIN    3
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    106

CRGB leds[NUM_LEDS];

#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120

// Presence States
#define ACTIVE_STATE 0
#define IDLE_STATE 1

// Presence State Pins (we don't need an idle state pin)
const int activeStatePin = 13;

void setup() {
  delay(3000); // 3 second delay for recovery
  Serial.begin(9600);
  Serial.println("STARTING...");
  
  pinMode(activeStatePin, INPUT);

  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // Set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
}

boolean isActive = false;

void loop()
{
  isActive = digitalRead(activeStatePin) == HIGH;
  Serial.print("isActive: ");
  Serial.println(isActive);

  if (!isActive) {
    active_state_display();
  }
  else {
    idle_state_display();
  }
  FastLED.show();  
  FastLED.delay(1000/FRAMES_PER_SECOND); 
}

void active_state_display () {
  fill_solid(leds, NUM_LEDS, CRGB::White);
}

void idle_state_display () {
  fill_solid(leds, NUM_LEDS, CRGB::Blue);
}