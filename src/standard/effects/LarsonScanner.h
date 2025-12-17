#ifndef LARSON_SCANNER_H
#define LARSON_SCANNER_H

#include "effects.h"
#include <Adafruit_NeoMatrix.h>

class LarsonScanner : public Effect {
public:
    // Constructor to pass in the matrix object and any initial settings
    LarsonScanner(Adafruit_NeoMatrix& matrixRef, int height, int larsonHeight, const uint16_t (&colors)[3][3], int* speedPtr, int* brightnessPtr, float* bkbrightnessPtr, int* colorIndexPtr);

    // Implementation of the base class pure virtual function
    void update(unsigned long deltaTime) override;

private:
    // Reference to the main matrix object
    Adafruit_NeoMatrix& matrix; 
    
    // Pointers to the global variables you want to control the effect
    int* speed;
    int* brightness;
    float* bkbrightness;
    int* colour_index;

    // Local state variables for the scanner movement
    int scannerpos;
    int scannerdir;

    // Constants
    const uint16_t (&coloursRef)[3][3];
    const int HEIGHT;
    const int LARSON_HEIGHT;
    
    unsigned long last_update_time;
};

#endif // LARSON_SCANNER_H