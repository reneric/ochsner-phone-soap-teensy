#include <FastLED.h>

FASTLED_USING_NAMESPACE

#define DATA_PIN    3
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    106

CRGB leds[NUM_LEDS];

#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120

// The time (ms) for the "complete" state
const int COMPLETE_STATE_DELAY = 5000; // 5 seconds

// Presence States
const int IDLE_STATE = 0;
const int ACTIVE_STATE = 1;
const int COMPLETE_STATE = 2;

// Presence State Pins (we don't need an idle state pin)
const int activeStatePin = 13;

int currentState = 0;
long completeStateTimer = 0;

void setup() {
  delay(3000); // 3 second delay for recovery
  Serial.begin(9600);
  Serial.println("STARTING...");
  
  pinMode(activeStatePin, INPUT);

  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // Set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
  currentState = 0;
}

void loop()
{
  stateMachine();
  FastLED.show();  
  FastLED.delay(1000/FRAMES_PER_SECOND);
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
  Serial.println("ACTIVE");
  fill_solid(leds, NUM_LEDS, CRGB::Blue);
}

void setComplete () {
  Serial.println("COMPLETE");
  fill_solid(leds, NUM_LEDS, CRGB::Green);
}

void setIdle () {
  Serial.println("IDLE");
  fill_solid(leds, NUM_LEDS, CRGB::White);
}

