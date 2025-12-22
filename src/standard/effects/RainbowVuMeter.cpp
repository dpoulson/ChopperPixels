#include "RainbowVuMeter.h"

// The maximum height the bar can fill, starting from the bottom of the matrix (HEIGHT-1)
// up to just below the top filled line (HEIGHT - LARSON_HEIGHT)
#define MAX_BAR_HEIGHT (LARSON_HEIGHT) 

// Constructor (similar to LarsonScanner)
RainbowVuMeter::RainbowVuMeter(Adafruit_NeoMatrix& matrixRef, int height, int larsonHeight, const uint16_t (&colors)[NUM_COLOURS][3], int* speedPtr, int* brightnessPtr, float* bkbrightnessPtr, int* colorIndexPtr)
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

void RainbowVuMeter::update(unsigned long deltaTime) {
    if (millis() < last_update_time + *speed) {
        return; 
    }
    last_update_time = millis();

    const int WIDTH = matrix.width();
    matrix.setBrightness(*brightness);
    
    // Background color remains based on your global selection
    int c_index = *colour_index;
    uint32_t bkColor = matrix.Color(
        int(coloursRef[c_index][0] / (*bkbrightness / 10.0)), 
        int(coloursRef[c_index][1] / (*bkbrightness / 10.0)), 
        int(coloursRef[c_index][2] / (*bkbrightness / 10.0))
    );

    matrix.fillScreen(bkColor);

    // 1. Draw the top line with the "current" rainbow start color
    uint32_t topColor = matrix.ColorHSV(hueOffset);
    matrix.fillRect(0, 0, WIDTH, HEIGHT - LARSON_HEIGHT, topColor); 

    // 2. Draw the VU Meter Bar with a gradient
    for (int y = 0; y < barHeight; y++) {
        // Calculate a unique hue for every row
        // 65536 / HEIGHT gives a full rainbow spread across the strip
        uint16_t pixelHue = hueOffset + (y * (65536 / HEIGHT));
        uint32_t gradColor = matrix.ColorHSV(pixelHue);
        
        // Draw a horizontal line for this row of the bar
        // Y coordinate is (HEIGHT - 1) - y because we fill from bottom up
        matrix.drawFastHLine(0, (HEIGHT - 1) - y, WIDTH, gradColor);
    }
    
    matrix.show();
    
    // 3. Update Logic
    barHeight += barDir;
    if (barHeight >= MAX_BAR_HEIGHT) { barHeight = MAX_BAR_HEIGHT; barDir = -1; }
    if (barHeight <= 0) { barHeight = 0; barDir = 1; }

    // 4. Advance the rainbow hue (change the speed of cycling here)
    hueOffset += 1000; 
}
