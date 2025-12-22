#ifndef REVERSE_VUMETER_H
#define REVERSE_VUMETER_H

#include "effects.h"
#include <Adafruit_NeoMatrix.h>

class ReverseVuMeter : public Effect {
public:
    // Constructor
    ReverseVuMeter(Adafruit_NeoMatrix& matrixRef, int height, int larsonHeight, const uint16_t (&colors)[NUM_COLOURS][3], int* speedPtr, int* brightnessPtr, float* bkbrightnessPtr, int* colorIndexPtr);

    // Implementation of the base class function
    void update(unsigned long deltaTime) override;

private:
    Adafruit_NeoMatrix& matrix;
    
    // Pointers to the global settings variables
    int* speed;
    int* brightness;
    float* bkbrightness;
    int* colour_index;

    // Constants
    const uint16_t (&coloursRef)[NUM_COLOURS][3];
    const int HEIGHT;
    const int LARSON_HEIGHT; // The segment of the matrix for the VU meter

    // Local state variables for the VU meter movement
    int barHeight; // Current height of the filled bar segment
    int barDir;    // Direction of movement: 1 (filling up) or -1 (emptying)
    
    unsigned long last_update_time;
};

#endif // REVERSE_VUMETER_H