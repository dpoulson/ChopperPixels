#ifndef COMMS_H
#define COMMS_H

#include <Arduino.h>
#include <Wire.h> // For I2C
#include <functional> // For std::function to handle callbacks

// Define constants used by the comms module
const int MAX_COMMAND_LENGTH = 32;

// Define a type for the function that will process received commands.
// This is a "callback" function that the Comms class will call
// whenever a complete command is received via I2C or Serial.
using CommandCallback = std::function<void(char* buffer)>;

class Comms {
public:
    // Constructor
    Comms(int i2cAddr, CommandCallback callback);

    // Initialization for all communication interfaces
    void begin();

    // Must be called repeatedly in loop() to check for incoming serial data
    void checkSerial();

    // Must be called repeatedly in loop() to check for incoming I2C command flag
    void checkI2C();

private:
    const int I2C_ADDR;
    CommandCallback commandCallback; // Stores the function pointer/lambda

    // I2C/Serial buffer state
    volatile bool i2cCommandReady = false;
    int serialBufferIndex = 0;
    char CommandBuffer[MAX_COMMAND_LENGTH];
    
    // Static methods are required for Wire.onReceive. 
    // They cannot access instance members directly, so we use a static pointer.
    static Comms* instance;
    static void staticReceiveEvent(int byteCount);

    // The actual, non-static handler
    void receiveEvent(int byteCount);
};

#endif // COMMS_H