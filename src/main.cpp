#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_GFX.h>
#include <Arduino.h>
#include <EEPROM.h>

#define EEPROM_SIZE 16

#define PIN 12             // Output pin for led board (6 = RLD)
#define WIDTH 2            // Width of LED matrix
#define HEIGHT 26          // Height of LED matrix
#define LARSON_HEIGHT 25   // Number of rows for larson scanner
#define OFF_FACTOR 5       // How much dimmer the 'off' LEDs will be (takes brightness and divides by this number)

#define PWM_CHECK 500      // How often to ch

#define DEFAULT_SPEED 30

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(WIDTH, HEIGHT, PIN,
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
bool rx_connected = true;
long int last_loop = 0;
long int last_check = 0;

volatile unsigned long pulseStartTime = 0;
volatile unsigned long pulseEndTime = 0;
volatile unsigned long pulseDuration = 0;
volatile bool pulseComplete = false;
byte interruptPin = 2;
byte channelAmount = 1;

const int numReadings = 30;
int readings[numReadings];
int readIndex = 0;
long total = 0;
int average = 0;
bool save_mode = true;

void move_bar() {
  matrix.setBrightness(brightness);
  matrix.fillScreen(matrix.Color(
      int(colours[colour_index][0]/(bkbrightness/100)), 
      int(colours[colour_index][1]/(bkbrightness/100)), 
      int(colours[colour_index][2]/(bkbrightness/100)))); // Clear Screen
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
  last_loop = millis();

  speed = map(analogRead(A1), 0, 890, 100, 0);
  brightness = map(analogRead(A2), 0, 890, 100, 0);

  Serial.print("Speed: ");
  Serial.println(speed);
  Serial.print("Brightness: ");
  Serial.println(brightness); 
  Serial.print("Colour Index: ");
  Serial.println(colour_index);
  
  delay(1000); // Allow time for RX to turn on.
}

void loop() {

  //speed = map(analogRead(A2), 0, 890, 100, 0);
  

  if (millis() > last_loop + speed) {
    
    // Tune the first two numbers depending on the signal from the RC system
    speed = map(analogRead(A1), 0, 890, 100, 0);

    brightness = map(analogRead(A2), 0, 890, 100, 0);
    bkbrightness = map(analogRead(A3), 0, 890, 1000, 0);

    Serial.print(speed);
    Serial.print(",");
    Serial.print(brightness);
    Serial.print(",");
    Serial.print(bkbrightness);
    Serial.print(",");
    Serial.print(analogRead(A3));
    Serial.println();
    
    move_bar();
    last_check = millis();
  }
   
}


