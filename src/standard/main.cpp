#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_GFX.h>
#include <Arduino.h>
#include <Wire.h>
#include <EEPROM.h>

#define DEBUG 0

const int I2C_ADDR = 0x08;

#define LADD_PIN 15        // Output pin for led board
#define PERI_PIN 2         // Output pin for Periscope
#define WIDTH 2            // Width of LED matrix
#define HEIGHT 26          // Height of LED matrix
#define LARSON_HEIGHT 25   // Number of rows for larson scanner

#define DEFAULT_SPEED 30

#define RC_PIN 35 // Jumper for enabling/disabling remote control
#define PT_PIN 32 // Trigger to fire off periscope LED

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(WIDTH, HEIGHT, LADD_PIN,
  NEO_MATRIX_TOP     + NEO_MATRIX_RIGHT +
  NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG,
  NEO_GRB            + NEO_KHZ800);

const uint16_t colours[3][3] = {
  {255,0,0}, // Red
  {0,255,0}, // Green
  {0,0,255}  // Blue
};

int speed = DEFAULT_SPEED;
int brightness = 40;
float bkbrightness = 5;
int colour_index = 0;
int scannerpos = HEIGHT-LARSON_HEIGHT;
int scannerdir = 1;
long int last_loop = 0;
bool rc_init_state = false;
static unsigned long rc_init_debounce = 0;
const unsigned int debounceDelay = 200; // Debounce delay in milliseconds


volatile bool i2cCommandReady = false;
int serialBufferIndex = 0;
const int MAX_COMMAND_LENGTH = 32;
char CommandBuffer[MAX_COMMAND_LENGTH];

struct Settings {
  int speed;
  int brightness;
  float bkbrightness;
  int colour_index;
};

Settings currentSettings;

#define EEPROM_SIZE sizeof(Settings) 


void receiveEvent(int byteCount) {
  // Read all incoming bytes into the buffer
  int i = 0;
  while (Wire.available() && i < sizeof(CommandBuffer) - 1) {
    CommandBuffer[i++] = Wire.read();
  }
  CommandBuffer[i] = '\0'; // Null-terminate the string

  i2cCommandReady = true; // Set flag
}

void parseAndExecute(char buffer[MAX_COMMAND_LENGTH]) {
  Serial.print("Command Buffer: ");
  Serial.println(buffer);
}

// This runs in the main loop
void checkI2C() {
  if (i2cCommandReady) {
    parseAndExecute(CommandBuffer);

    i2cCommandReady = false; // Reset flag
  }
}

void checkSerial() {
  if (Serial2.available() > 0) {
    char incomingByte = Serial2.read();
    if (incomingByte == '\n' || incomingByte == '\r') {
      CommandBuffer[serialBufferIndex] = '\0';
      if (serialBufferIndex > 0) {
        Serial.print("Received Serial Command: ");
        Serial.println(CommandBuffer);
        parseAndExecute(CommandBuffer); 
      }

      serialBufferIndex = 0;

    } else {
      if (serialBufferIndex < MAX_COMMAND_LENGTH - 1) {
        CommandBuffer[serialBufferIndex] = incomingByte;
        serialBufferIndex++;
      } else {
        Serial.println("Serial Command Too Long! Discarding...");
        serialBufferIndex = 0;
      }
    }
  }
}

void move_bar() {
  matrix.setBrightness(brightness);
  matrix.fillScreen(matrix.Color(
      int(colours[colour_index][0]/(bkbrightness/10)), 
      int(colours[colour_index][1]/(bkbrightness/10)), 
      int(colours[colour_index][2]/(bkbrightness/10)))); // Clear Screen
  matrix.drawRect(0,0,WIDTH,HEIGHT-LARSON_HEIGHT, matrix.Color(colours[colour_index][0],colours[colour_index][1],colours[colour_index][2]));      // Draw top light
  matrix.drawRect(0, scannerpos, WIDTH,1, matrix.Color(colours[colour_index][0],colours[colour_index][1],colours[colour_index][2]) ); // Draw scanner line
  matrix.show();
  scannerpos = scannerpos + scannerdir;
  
  if (scannerpos >= HEIGHT) {
    scannerpos = HEIGHT-2;
    scannerdir = -1; 
  }
  if (scannerpos <= HEIGHT-LARSON_HEIGHT) {
    scannerpos = HEIGHT-LARSON_HEIGHT;
    scannerdir = 1; 
  }

  last_loop = millis();
  
}

void setup() {

  matrix.begin();
  Serial.begin(115200);
  Serial.println("Booting.... ");
  Serial.println("Initialising EEPROM");
  EEPROM.begin(EEPROM_SIZE);
  pinMode(RC_PIN, INPUT_PULLUP);
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

  Wire.begin(I2C_ADDR); // Start as I2C Slave at address 0x08
  Wire.onReceive(receiveEvent); // Register the interrupt handler function

}

void saveSettings() {
  Serial.println("Saving settings....");
  currentSettings.speed = speed;
  currentSettings.brightness = brightness;
  currentSettings.bkbrightness = bkbrightness;
  currentSettings.colour_index = colour_index;
  
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

  Serial.println("....Done");
}

void loop() {

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

  // Main LED panel loop
  if (millis() > last_loop + speed) {

    if (digitalRead(RC_PIN) == LOW) { // RC PIN not jumpered, so read from trim pots
      speed = map(analogRead(25), 0, 4096, 100, 0);
      brightness = map(analogRead(26), 0, 4096, 100, 0);
      bkbrightness = map(analogRead(27), 0, 4096, 100, 1);
    }
    move_bar();
  }

  checkSerial();
  checkI2C();

  // If rc_init_state changes, then save settings to eeprom
  if (rc_init_state != digitalRead(RC_PIN)) {
    if ((millis() - rc_init_debounce) > debounceDelay) {
      saveSettings();
      rc_init_state = digitalRead(RC_PIN);
      rc_init_debounce = millis();
    }
  }
}

