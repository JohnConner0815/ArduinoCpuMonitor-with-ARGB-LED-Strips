#include <FastLED.h>
#include <EEPROM.h> // Included for saving settings

// --- PIN DEFINITIONS ---
#define DATA_PIN 6      
#define BTN1_PIN 3      
#define BTN2_PIN 4      

// --- HARDWARE CONFIGURATION ---
#define NUM_LEDS 26     
#define BRIGHTNESS_STEPS 6
#define DEBOUNCE_DELAY 80 

// --- EEPROM ADDRESSES ---
#define EEPROM_PROFILE 0
#define EEPROM_BRIGHTNESS 1

// --- INITIALIZATION ---
CRGB leds[NUM_LEDS];

uint8_t brightnessLevels[BRIGHTNESS_STEPS] = {0, 8, 33, 81, 155, 255};

int cpu = 0;
int activeLeds = 0;
int currentProfile = 1;   
int brightnessIndex = 5;  

int btn1State = HIGH;
int lastBtn1State = HIGH;
unsigned long lastBtn1DebounceTime = 0;

int btn2State = HIGH;
int lastBtn2State = HIGH;
unsigned long lastBtn2DebounceTime = 0;

void setup() {
  Serial.begin(19200);
  
  pinMode(BTN1_PIN, INPUT_PULLUP);
  pinMode(BTN2_PIN, INPUT_PULLUP);

  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.clear();

  // --- READ SAVED SETTINGS FROM MEMORY ---
  currentProfile = EEPROM.read(EEPROM_PROFILE);
  // Fallback to 1 if the memory is empty/corrupted (first ever boot)
  if (currentProfile < 1 || currentProfile > 6) currentProfile = 1; 
  
  brightnessIndex = EEPROM.read(EEPROM_BRIGHTNESS);
  // Fallback to 5 (100%) if the memory is empty/corrupted
  if (brightnessIndex < 0 || brightnessIndex >= BRIGHTNESS_STEPS) brightnessIndex = 5;

  // --- STARTUP SEQUENCE ---
  // Force a visible brightness for the startup sequence (60%)
  FastLED.setBrightness(81); 
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::White;
    FastLED.show();
    delay(50);
  }
  delay(500); 
  FastLED.clear();
  
  // Apply the SAVED brightness right after the sequence finishes
  FastLED.setBrightness(brightnessLevels[brightnessIndex]);
  FastLED.show();
}

void loop() {
  if (Serial.available()) {
    cpu = Serial.parseInt();
    cpu = constrain(cpu, 0, 100); 
  }

  activeLeds = map(cpu, 0, 100, 0, NUM_LEDS);

  // --- BUTTON 1 (PROFILE) ---
  int btn1Reading = digitalRead(BTN1_PIN);
  if (btn1Reading != lastBtn1State) {
    lastBtn1DebounceTime = millis();
  }
  if ((millis() - lastBtn1DebounceTime) > DEBOUNCE_DELAY) {
    if (btn1Reading != btn1State) {
      btn1State = btn1Reading;
      if (btn1State == LOW) {
        currentProfile++;
        if (currentProfile > 6) currentProfile = 1;
        // SAVE TO MEMORY
        EEPROM.update(EEPROM_PROFILE, currentProfile); 
      }
    }
  }
  lastBtn1State = btn1Reading;

  // --- BUTTON 2 (BRIGHTNESS) ---
  int btn2Reading = digitalRead(BTN2_PIN);
  if (btn2Reading != lastBtn2State) {
    lastBtn2DebounceTime = millis();
  }
  if ((millis() - lastBtn2DebounceTime) > DEBOUNCE_DELAY) {
    if (btn2Reading != btn2State) {
      btn2State = btn2Reading;
      if (btn2State == LOW) {
        brightnessIndex++;
        if (brightnessIndex >= BRIGHTNESS_STEPS) brightnessIndex = 0;
        FastLED.setBrightness(brightnessLevels[brightnessIndex]);
        // SAVE TO MEMORY
        EEPROM.update(EEPROM_BRIGHTNESS, brightnessIndex);
      }
    }
  }
  lastBtn2State = btn2Reading;

  // --- UPDATE LEDs ---
  FastLED.clear();

  if (currentProfile == 6) {
    // Profile 6: All off
  } 
  else {
    for (int i = 0; i < activeLeds; i++) {
      switch (currentProfile) {
        case 1: leds[i] = CRGB::White; break;
        case 2: leds[i] = CRGB::Blue; break;
        case 3: leds[i] = CRGB::Cyan; break;
        case 4: 
          leds[i] = getLoadColor((float)i / (NUM_LEDS - 1)); 
          break;
        case 5: 
          leds[i] = getLoadColor((float)cpu / 100.0); 
          break;
      }
    }
  }

  FastLED.show();
}

CRGB getLoadColor(float percent) {
  percent = constrain(percent, 0.0, 1.0);
  
  if (percent <= 0.60) {
    return CRGB(0, 255, 0); 
  } 
  else if (percent <= 0.70) {
    float t = (percent - 0.60) / 0.10; 
    return CRGB((uint8_t)(255 * t), 255, 0);
  } 
  else if (percent <= 0.80) {
    return CRGB(255, 255, 0); 
  } 
  else if (percent <= 0.90) {
    float t = (percent - 0.80) / 0.10;
    return CRGB(255, (uint8_t)(255 * (1.0 - t)), 0);
  } 
  else {
    return CRGB(255, 0, 0); 
  }
}
