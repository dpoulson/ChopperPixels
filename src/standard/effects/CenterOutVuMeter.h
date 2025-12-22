#ifndef CENTER_OUT_VUMETER_H
#define CENTER_OUT_VUMETER_H

#include "effects.h"
#include <Adafruit_NeoMatrix.h>

class CenterOutVuMeter : public Effect {
public:
    // Constructor
    CenterOutVuMeter(Adafruit_NeoMatrix& matrixRef, int height, int larsonHeight, const uint16_t (&colors)[NUM_COLOURS][3], int* speedPtr, int* brightnessPtr, float* bkbrightnessPtr, int* colorIndexPtr);

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
    const int LARSON_HEIGHT; 
    
    // Local state variables for the VU meter movement
    int barExtent; // Current number of rows filled *out* from the center point
    int barDir;    // Direction of movement: 1 (filling up) or -1 (emptying)
    
    // Calculated constants
    const int MAX_EXTENT;     // Max number of rows one half can fill
    const int TOP_LIGHT_ROWS; // The rows at the top that are always filled
    const int CENTER_ROW;     // The row just below the middle (or the middle of the two center rows)

    unsigned long last_update_time;
};

#endif // CENTER_OUT_VUMETER_H