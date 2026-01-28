#ifndef EFFECTS_H
#define EFFECTS_H

#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_GFX.h>

#define NUM_COLOURS 6

// Forward declaration to avoid circular includes in more complex setups
class Adafruit_NeoMatrix;

class Effect {
public:
    // Virtual destructor is good practice for base classes
    virtual ~Effect() {} 

    // The core function all effects must implement.
    // deltaTime is the time since the last call, useful for frame-rate independence.
    virtual void update(unsigned long deltaTime) = 0;

    // Optional: a function to call when the effect is first activated
    virtual void setup() {}

    // Optional: a function to call when the effect is deactivated
    virtual void teardown() {}

    // You can also add functions here for setting parameters (speed, color, etc.)
};

#endif // EFFECT_H