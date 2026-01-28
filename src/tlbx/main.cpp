#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_GFX.h>
#include <Arduino.h>
#include <Wire.h>
#include <EEPROM.h>

#include "ReelTwo.h"
#include "dome/Logics.h"
#include "dome/LogicEngineController.h"

#include "effects/effects.h"
#include "effects/LarsonScanner.h" 
#include "effects/Random.h"
#include "effects/VuMeter.h"
#include "effects/ReverseVuMeter.h"
#include "effects/CenterOutVuMeter.h"
#include "effects/RainbowVuMeter.h"

#include "Comms.h"

#define DEBUG 0

const int I2C_ADDR = 0x08;

#define LADD_PIN 15        // Output pin for led board
#define PERI_PIN 13        // Output pin for Periscope
#define EYES_PIN 4          // Output pin for eyes
#define WIDTH 2            // Width of LED matrix
#define HEIGHT 26          // Height of LED matrix
#define LARSON_HEIGHT 25   // Number of rows for larson scanner
#define PERI_LENGTH 1      // Define the length of the periscope LED string
#define EYES_LENGTH 2      // Length of string for eyes (default 2)

#define DEFAULT_SPEED 30

#define RC_PIN 35 // Jumper for enabling/disabling remote control
#define PT_PIN 32 // Trigger to fire off periscope LED

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(WIDTH, HEIGHT, LADD_PIN,
  NEO_MATRIX_TOP     + NEO_MATRIX_RIGHT +
  NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG,
  NEO_GRB            + NEO_KHZ800);

Adafruit_NeoPixel Peri(PERI_LENGTH, PERI_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel Eyes(EYES_LENGTH, EYES_PIN, NEO_GRB + NEO_KHZ800);

AstroPixelFLD<4> FLD(LogicEngineFLDDefault, 1);
AstroPixelFLDTLBX<5> RLD(LogicEngineFLDDefault, 1);

#define NUM_COLOURS 6

const uint16_t colours[NUM_COLOURS][3] = {
  {255,0,0}, // Red
  {0,255,0}, // Green
  {0,0,255},  // Blue
  {255,255,0}, // Yellow
  {255,0,255}, // Magenta
  {0,255,255}, // Cyan
};

enum EffectID {
  EFFECT_LARSON_SCANNER = 0,
  EFFECT_VU_METER = 1,
  EFFECT_REVERSE_VU_METER = 2,
  EFFECT_CENTER_OUT_VU_METER = 3,
  EFFECT_RANDOM = 4,
  EFFECT_RAINBOW_VU_METER=5,
  TOTAL_EFFECTS = 6 // Helper for counting
};

int speed = DEFAULT_SPEED;
int brightness = 40;
float bkbrightness = 5;
int colour_index = 0;
int peri_colour_index = 2;
int eyes_colour_index = 2;
int effect = 0;

long int last_loop = 0;
bool rc_init_state = false;
int last_pt_pin_state = HIGH;         // Stores the last *known* stable state of the pin
unsigned long pt_debounce = 0; // The last time the pin was checked/updated
static unsigned long rc_init_debounce = 0;
const unsigned int debounceDelay = 200; // Debounce delay in milliseconds

bool pt_pin_triggered = false; 

struct Settings {
  int speed;
  int brightness;
  float bkbrightness;
  int colour_index;
  int peri_colour_index;
  int effect;
  int eyes_colour_index;
};

Settings currentSettings;
Effect* currentEffect = nullptr; // Pointer to the currently running effect
unsigned long last_update_time = 0;

#define EEPROM_SIZE sizeof(Settings) 

void saveSettings() {
  Serial.println("Saving settings....");
  currentSettings.speed = speed;
  currentSettings.brightness = brightness;
  currentSettings.bkbrightness = bkbrightness;
  currentSettings.colour_index = colour_index;
  currentSettings.peri_colour_index = peri_colour_index;
  currentSettings.effect = effect;
  currentSettings.eyes_colour_index = eyes_colour_index;
  
  // 2. Write the entire struct to address 0
  EEPROM.put(0, currentSettings); 
  EEPROM.commit();

  Serial.print("Speed: ");
  Serial.print(speed);

  Serial.print(" brightness: ");
  Serial.print(brightness);

  Serial.print(" bkbrightness: ");
  Serial.print(bkbrightness);
  
  Serial.print(" colour_index: ");
  Serial.print(colour_index);

  Serial.print(" peri_colour_index: ");
  Serial.print(peri_colour_index);

  Serial.print(" effect: ");
  Serial.print(effect);

  Serial.print(" eyes_colour_index: ");
  Serial.print(eyes_colour_index);

  Serial.println("....Done");
}

void switchEffect(int newEffectID) {
    if (newEffectID < 0 || newEffectID >= TOTAL_EFFECTS) {
        Serial.print("Error: Invalid effect ID ");
        Serial.println(newEffectID);
        return;
    }

    // 1. Teardown the current effect
    if (currentEffect != nullptr) {
        currentEffect->teardown(); // Call the effect's cleanup routine
        delete currentEffect;       // Free the memory
        currentEffect = nullptr;
    }

    // 2. Create the new effect based on the ID
    Serial.print("Switching to effect ID: ");
    Serial.println(newEffectID);
    
    // Pass matrix reference and pointers to global setting variables (speed, brightness, etc.)
    switch (newEffectID) {
        case EFFECT_LARSON_SCANNER:
            currentEffect = new LarsonScanner(
                matrix, HEIGHT, LARSON_HEIGHT, colours, &speed, &brightness, &bkbrightness, &colour_index
            );
            break;

        case EFFECT_VU_METER:
            currentEffect = new VuMeter(
                matrix, HEIGHT, LARSON_HEIGHT, colours, &speed, &brightness, &bkbrightness, &colour_index
            );
            break;

        case EFFECT_REVERSE_VU_METER:
            currentEffect = new ReverseVuMeter(
                matrix, HEIGHT, LARSON_HEIGHT, colours, &speed, &brightness, &bkbrightness, &colour_index
            );
            break;

        case EFFECT_CENTER_OUT_VU_METER:
            currentEffect = new CenterOutVuMeter(
                matrix, HEIGHT, LARSON_HEIGHT, colours, &speed, &brightness, &bkbrightness, &colour_index
            );
            break;

        case EFFECT_RAINBOW_VU_METER:
            currentEffect = new RainbowVuMeter(
                matrix, HEIGHT, LARSON_HEIGHT, colours, &speed, &brightness, &bkbrightness, &colour_index
            );
            break;

        case EFFECT_RANDOM:
            currentEffect = new Random(
                matrix, HEIGHT, LARSON_HEIGHT, colours, &speed, &brightness, &bkbrightness, &colour_index
            );
            break;
    }

    // 3. Initialize the new effect
    if (currentEffect != nullptr) {
        currentEffect->setup();
        // Reset the timer for the first update call
        last_update_time = millis(); 
        effect = newEffectID;
    }
}

void parseAndExecute(char* buffer) {
  Serial.print("Command Buffer: ");
  Serial.println(buffer);
  
  // Use strncmp to check for prefixes
  if (strncmp(buffer, "EFF=", 4) == 0) {
      // Command format: EFF=X (e.g., EFF=2)
      int newEffectID = atoi(buffer + 4); // Convert the characters after "EFF=" to an integer
      switchEffect(newEffectID);
      
  } else if (strncmp(buffer, "SPEED=", 6) == 0) {
      // Command format: SPEED=X
      int newSpeed = atoi(buffer + 6);
      if (newSpeed >= 0 && newSpeed <= 100) {
          speed = newSpeed;
          Serial.print("Set Speed to: ");
          Serial.println(speed);
      }

  } else if (strncmp(buffer, "COLOUR=", 4) == 0) {
      // Command format: COLOUR=X
      int newColour = atoi(buffer + 7);
      if (newColour >= 1 && newColour <= NUM_COLOURS) {
          colour_index = newColour - 1;
          Serial.print("Set Colour to: ");
          Serial.println(colour_index);
      }

  } else if (strncmp(buffer, "EYES=", 4) == 0) {
      // Command format: EYES=X
      int newColour = atoi(buffer + 5);
      if (newColour >= 1 && newColour <= NUM_COLOURS) {
          eyes_colour_index = newColour - 1;
          Serial.print("Set Colour to: ");
          Serial.println(eyes_colour_index);
      }
  
  } else if (strncmp(buffer, "PERI=", 4) == 0) {
      // Command format: PERI=X
      int newColour = atoi(buffer + 5);
      if (newColour >= 1 && newColour <= NUM_COLOURS) {
          peri_colour_index = newColour - 1;
          Serial.print("Set Colour to: ");
          Serial.println(peri_colour_index);
      }
      
  } else if (strcmp(buffer, "SAVE") == 0) {
      // Command format: SAVE
      saveSettings();
      
  } else {
      Serial.println("Unknown Command.");
  }
}

Comms comms(I2C_ADDR, parseAndExecute);

void setup() {


  REELTWO_READY();
  RLD.selectScrollTextLeft("... TLBX ....", LogicEngineRenderer::kBlue, 0, 15);
  matrix.begin();
  Peri.begin();

  Serial.begin(115200);
  Serial.println("Booting.... ");
  Serial.println("Initialising EEPROM");
  EEPROM.begin(EEPROM_SIZE);
  pinMode(RC_PIN, INPUT_PULLUP);
  pinMode(PT_PIN, INPUT_PULLDOWN);
  last_pt_pin_state = digitalRead(PT_PIN);
  last_loop = millis();

  
  if (digitalRead(RC_PIN) == HIGH) { // RC Pin is jumpered, so read from EEPROM
    Serial.println("Loading defaults from EEPROM");
    // Read the entire struct at address 0
    EEPROM.get(0, currentSettings); 
    rc_init_state = true;
    // Now load the structure values into your global variables
    speed = currentSettings.speed;
    brightness = currentSettings.brightness;
    bkbrightness = currentSettings.bkbrightness;
    colour_index = currentSettings.colour_index;
    peri_colour_index = currentSettings.peri_colour_index;
    effect = currentSettings.effect;
    eyes_colour_index = currentSettings.eyes_colour_index;
    rc_init_state = true;
  } else {
    speed = map(analogRead(25), 0, 4096, 100, 0);
    brightness = map(analogRead(26), 0, 4096, 100, 0);
    bkbrightness = map(analogRead(27), 0, 4096, 100, 1);
    rc_init_state = false;
  }

  Serial.print("Speed: ");
  Serial.println(speed);
  Serial.print("Brightness: ");
  Serial.println(brightness); 
  Serial.print("Back Brightness: ");
  Serial.println(bkbrightness); 
  Serial.print("Colour Index: ");
  Serial.println(colour_index);

  comms.begin();

  switchEffect(effect);

}

void loop() {
  AnimatedEvent::process();
  //Serial.print("Peri Trigger: ");
  //Serial.println(digitalRead(PT_PIN));

  #if DEBUG == 1
    Serial.print(digitalRead(RC_PIN));
    Serial.print(", ");
    //Serial.print(millis());
    //Serial.print(", ");
    Serial.print(last_loop);
    Serial.print(", ");
    Serial.print(speed);
    Serial.print(", ");
    Serial.print(brightness);
    Serial.print(", ");
    Serial.print(bkbrightness);
    Serial.print(", ");
    Serial.print(analogRead(25));
    Serial.print(", ");
    Serial.print(int(colours[colour_index][0]/(bkbrightness)));    
    Serial.println();
  #endif

  if (currentEffect != nullptr) {
      unsigned long now = millis();
      unsigned long deltaTime = now - last_update_time;
      currentEffect->update(deltaTime); // Update the effect
      last_update_time = now;
  }

  if (digitalRead(RC_PIN) == LOW) { // RC PIN not jumpered, so read from trim pots
    speed = map(analogRead(25), 0, 4096, 100, 0);
    brightness = map(analogRead(26), 0, 4096, 100, 0);
    bkbrightness = map(analogRead(27), 0, 4096, 100, 1);
  }

  comms.checkSerial(); 
  comms.checkI2C();

  // If rc_init_state changes, then save settings to eeprom
  if (rc_init_state != digitalRead(RC_PIN)) {
    if ((millis() - rc_init_debounce) > debounceDelay) {
      saveSettings();
      rc_init_state = digitalRead(RC_PIN);
      rc_init_debounce = millis();
    }
  }


  int pt_read = digitalRead(PT_PIN);
  if ((millis() - pt_debounce) > debounceDelay) {
      if (pt_read != last_pt_pin_state) {
          if (pt_read == HIGH) {
            Serial.println("Periscope Triggered");
            for (int pixel = 0; pixel < PERI_LENGTH; pixel++) {
              Serial.print("Setting pixel:");
              Serial.print(pixel);
              Peri.setPixelColor(pixel, Peri.Color(255, 0, 0));  
            }
            Peri.show();
          }
          else if (pt_read == LOW) {
            Serial.println("Periscope Off");
            Peri.clear();
            Peri.show();
          }
          last_pt_pin_state = pt_read; 
      }

      pt_debounce = millis();
  }


}

