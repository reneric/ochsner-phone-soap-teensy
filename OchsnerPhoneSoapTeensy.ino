#include <FastLED.h>

#define DEBUG 0

FASTLED_USING_NAMESPACE

#define DATA_PIN    3
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    60

CRGB leds[NUM_LEDS];

#define BRIGHTNESS         200
#define FRAMES_PER_SECOND  120

// The time (ms) for the "complete" state
const int COMPLETE_STATE_DELAY = 5000; // 5 seconds

// Presence States
const int IDLE_STATE = 0;
const int ACTIVE_STATE = 1;
const int COMPLETE_STATE = 2;

// Presence State Pins (we don't need an idle state pin)
const int activeStatePin = 13;
const int togglePin = 14;

int currentState = 0;
long completeStateTimer = 0;
boolean deviceOff ();

CRGBPalette16 currentPalette;

void setup() {
  delay(3000); // 3 second delay for recovery
  Serial.begin(9600);
  Serial.println("STARTING...");
  
  pinMode(activeStatePin, INPUT);
  pinMode(togglePin, INPUT);

  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // Set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
  currentState = 0;
}

void loop()
{
  if (digitalRead(togglePin) == LOW) {
    FastLED.clear();
  }
  else {
    stateMachine();
  }
  
  FastLED.show();  
  FastLED.delay(1000/FRAMES_PER_SECOND);
}

boolean deviceOff () {
  return digitalRead(togglePin) == LOW;
}

void stateMachine () {
  // Get the current sensor state
  int tempState = getState();

  if (tempState != currentState ) {
    boolean phoneSoapCompleted = tempState == IDLE_STATE && currentState == ACTIVE_STATE;
    // If the last currentState was "ACTIVE" and the new state is "IDLE",
    // set state to "COMPLETE" and start the completion timer.
    if (phoneSoapCompleted) {
      completeStateTimer = millis();
      currentState = COMPLETE_STATE;
    }

    boolean inCompletionMode = currentState == COMPLETE_STATE && (millis() - completeStateTimer < COMPLETE_STATE_DELAY);
    
    currentState = inCompletionMode ? COMPLETE_STATE : tempState;
  }
  switch (currentState) {
    case ACTIVE_STATE:
      setActive();
      break;
    case COMPLETE_STATE:
      setComplete();
      break;
    default:
      setIdle();
      break;
  }
}

int getState () {
  return digitalRead(activeStatePin) == HIGH;
}

void setActive () {
#if DEBUG == 1  
  Serial.println("ACTIVE");
#endif
  // fill_solid(leds, NUM_LEDS, CRGB::Blue);
    
    static uint8_t startIndex = 0;
    startIndex = startIndex + 1; /* motion speed */
    
    FillLEDsFromPaletteColors( startIndex);
}

void setComplete () {
#if DEBUG == 1
  Serial.println("COMPLETE");
#endif
  fill_solid(leds, NUM_LEDS, CRGB::Green);
}

void setIdle () {
#if DEBUG == 1
  Serial.println("IDLE");
#endif
  fill_solid(leds, NUM_LEDS, CRGB::Gray);
}

uint8_t gHue = 0;

void fadeall() { for(int i = 0; i < NUM_LEDS; i++) { leds[i].nscale8(250); } }

void cylon () {
  static uint8_t hue = 0;
  Serial.print("x");
  // First slide the led in one direction
  for(int i = 0; i < NUM_LEDS; i++) {
    // Set the i'th led to red 
    leds[i] = CHSV(hue++, 255, 255);
    // Show the leds
    FastLED.show(); 
    // now that we've shown the leds, reset the i'th led to black
    // leds[i] = CRGB::Black;
    fadeall();
    // Wait a little bit before we loop around and do it again
    delay(10);
  }
  Serial.print("x");

  // Now go in the other direction.  
  for(int i = (NUM_LEDS)-1; i >= 0; i--) {
    // Set the i'th led to red 
    leds[i] = CHSV(hue++, 255, 255);
    // Show the leds
    FastLED.show();
    // now that we've shown the leds, reset the i'th led to black
    // leds[i] = CRGB::Black;
    fadeall();
    // Wait a little bit before we loop around and do it again
    delay(10);
  }
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS-1 );
  leds[pos] += CHSV( gHue, 255, 192);
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

void FillLEDsFromPaletteColors( uint8_t colorIndex)
{
    for( int i = 0; i < NUM_LEDS; i++) {
        leds[i] = ColorFromPalette( SetupActivePalette(), colorIndex, BRIGHTNESS, LINEARBLEND);
        colorIndex += 5;
    }
}

CRGBPalette16 SetupActivePalette()
{
    CRGB blue = CHSV( HUE_BLUE, 255, 255);
    CRGB darkBlue  = CRGB::DarkBlue;
    CRGB black  = CRGB::DarkBlue;
    
    return CRGBPalette16(
                                   blue,  blue,  black,  black,
                                   darkBlue, darkBlue, black,  black,
                                   blue,  blue,  black,  black,
                                   darkBlue, darkBlue, black,  black );
}


