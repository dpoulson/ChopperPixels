#include "LarsonScanner.h"

// Note: We use references/pointers to access the global variables for settings.
LarsonScanner::LarsonScanner(Adafruit_NeoMatrix& matrixRef, int height, int larsonHeight, const uint16_t (&colors)[3][3], int* speedPtr, int* brightnessPtr, float* bkbrightnessPtr, int* colorIndexPtr)
    : matrix(matrixRef), // Initialize the reference member
      coloursRef(colors), // Initialize the reference to the colors array
      HEIGHT(height),
      LARSON_HEIGHT(larsonHeight),
      speed(speedPtr),
      brightness(brightnessPtr),
      bkbrightness(bkbrightnessPtr),
      colour_index(colorIndexPtr),
      scannerpos(height - larsonHeight),
      scannerdir(1),
      last_update_time(0)
{
    // The matrix width is defined in the matrix object, no need to pass it here.
}

void LarsonScanner::update(unsigned long deltaTime) {
    // Use speed (in ms) to control the delay between updates
    if (millis() < last_update_time + *speed) {
        return; // Not enough time has passed
    }
    last_update_time = millis();

    // The original logic from move_bar()
    const int WIDTH = matrix.width();
    int c_index = *colour_index;

    matrix.setBrightness(*brightness);
    
    // Calculate the background color intensity
    uint32_t bkColor = matrix.Color(
        int(coloursRef[c_index][0] / (*bkbrightness / 10.0)), 
        int(coloursRef[c_index][1] / (*bkbrightness / 10.0)), 
        int(coloursRef[c_index][2] / (*bkbrightness / 10.0))
    );
    
    // Foreground color
    uint32_t fgColor = matrix.Color(
        coloursRef[c_index][0],
        coloursRef[c_index][1],
        coloursRef[c_index][2]
    );

    matrix.fillScreen(bkColor); // Clear Screen (with background color)
    
    // Draw top light
    matrix.drawRect(0, 0, WIDTH, HEIGHT - LARSON_HEIGHT, fgColor); 
    
    // Draw scanner line
    matrix.drawRect(0, scannerpos, WIDTH, 1, fgColor); 
    
    matrix.show();
    
    // Move and bounce logic
    scannerpos = scannerpos + scannerdir;
    
    if (scannerpos >= HEIGHT) {
        scannerpos = HEIGHT - 2;
        scannerdir = -1; 
    }
    if (scannerpos <= HEIGHT - LARSON_HEIGHT) {
        scannerpos = HEIGHT - LARSON_HEIGHT;
        scannerdir = 1; 
    }
}