#include "ReverseVuMeter.h"

// The maximum height the bar can fill, starting from the bottom of the matrix (HEIGHT-1)
// up to just below the top filled line (HEIGHT - LARSON_HEIGHT)
#define MAX_BAR_HEIGHT (LARSON_HEIGHT) 

// Constructor (similar to LarsonScanner)
ReverseVuMeter::ReverseVuMeter(Adafruit_NeoMatrix& matrixRef, int height, int larsonHeight, const uint16_t (&colors)[NUM_COLOURS][3], int* speedPtr, int* brightnessPtr, float* bkbrightnessPtr, int* colorIndexPtr)
    : matrix(matrixRef),
      coloursRef(colors),
      HEIGHT(height),
      LARSON_HEIGHT(larsonHeight),
      speed(speedPtr),
      brightness(brightnessPtr),
      bkbrightness(bkbrightnessPtr),
      colour_index(colorIndexPtr),
      barHeight(0), // Start empty
      barDir(1),    // Start filling up
      last_update_time(0)
{
}

void ReverseVuMeter::update(unsigned long deltaTime) {
    // Control the update rate using the 'speed' setting
    if (millis() < last_update_time + *speed) {
        return; 
    }
    last_update_time = millis();

    const int WIDTH = matrix.width();
    int c_index = *colour_index;

    matrix.setBrightness(*brightness);
    
    // Calculate colors (same as LarsonScanner)
    uint32_t bkColor = matrix.Color(
        int(coloursRef[c_index][0] / (*bkbrightness / 10.0)), 
        int(coloursRef[c_index][1] / (*bkbrightness / 10.0)), 
        int(coloursRef[c_index][2] / (*bkbrightness / 10.0))
    );
    uint32_t fgColor = matrix.Color(
        coloursRef[c_index][0],
        coloursRef[c_index][1],
        coloursRef[c_index][2]
    );

    matrix.fillScreen(bkColor); // Clear Screen (with background color)
    
    // 1. Draw the perpetually filled top line
    // It's a rectangle from (0, 0) with a height of (HEIGHT - LARSON_HEIGHT) which is 1
    matrix.fillRect(0, 0, WIDTH, HEIGHT - LARSON_HEIGHT, fgColor); 

    // 2. Draw the VU Meter Bar
    if (barHeight > 0) {
        // The bar starts at the bottom of the matrix (HEIGHT - 1) and goes up (decreases the Y-coordinate)
        // Y-coordinate of the bar's top edge: HEIGHT - barHeight
        // Height of the filled rectangle: barHeight
        matrix.fillRect(0, HEIGHT - barHeight, WIDTH, barHeight, fgColor); 
    }
    
    matrix.show();
    
    // 3. Update Bar Height (Movement Logic)
    barHeight += barDir;
    
    // Check bounds and reverse direction
    if (barHeight >= MAX_BAR_HEIGHT) {
        barHeight = MAX_BAR_HEIGHT; // Cap at max
        barDir = -1; // Start emptying
    }
    if (barHeight <= 0) {
        barHeight = 0; // Cap at min (empty)
        barDir = 1; // Start filling up
    }
}