#include "CenterOutVuMeter.h"

// Constructor
CenterOutVuMeter::CenterOutVuMeter(Adafruit_NeoMatrix& matrixRef, int height, int larsonHeight, const uint16_t (&colors)[3][3], int* speedPtr, int* brightnessPtr, float* bkbrightnessPtr, int* colorIndexPtr)
    : matrix(matrixRef),
      coloursRef(colors),
      HEIGHT(height),
      LARSON_HEIGHT(larsonHeight),
      speed(speedPtr),
      brightness(brightnessPtr),
      bkbrightness(bkbrightnessPtr),
      colour_index(colorIndexPtr),
      barExtent(0), // Start empty
      barDir(1),    // Start filling up
      TOP_LIGHT_ROWS(height - larsonHeight), // 26 - 25 = 1
      CENTER_ROW(TOP_LIGHT_ROWS + (LARSON_HEIGHT / 2) + 1), // 1 + 12 + 1 = 14
      MAX_EXTENT(LARSON_HEIGHT / 2), // 12
      last_update_time(0)
{
}

void CenterOutVuMeter::update(unsigned long deltaTime) {
    // Control the update rate using the 'speed' setting
    if (millis() < last_update_time + *speed) {
        return; 
    }
    last_update_time = millis();

    const int WIDTH = matrix.width();
    int c_index = *colour_index;

    matrix.setBrightness(*brightness);
    
    // Calculate colors
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
    
    // 1. Draw the perpetually filled top line (Row 0)
    matrix.fillRect(0, 0, WIDTH, TOP_LIGHT_ROWS, fgColor); 

    // 2. Draw the Center-Out Meter Bars (only if extent > 0)
    if (barExtent > 0) {
        // --- Bar extending UP from the center ---
        matrix.fillRect(0, CENTER_ROW - barExtent, WIDTH, barExtent, fgColor);
        // --- Bar extending DOWN from the center ---
        matrix.fillRect(0, CENTER_ROW, WIDTH, barExtent, fgColor);
    }
    
    matrix.show();
    
    // 3. Update Bar Extent (Movement Logic)
    barExtent += barDir;
    
    // Check bounds and reverse direction
    if (barExtent > MAX_EXTENT) {
        barExtent = MAX_EXTENT; // Cap at max
        barDir = -1; // Start emptying
    }
    if (barExtent <= 0) {
        barExtent = 0; // Cap at min (empty)
        barDir = 1; // Start filling up
    }

}