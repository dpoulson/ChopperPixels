#include "Comms.h"

// Initialize the static instance pointer
Comms* Comms::instance = nullptr;

// Constructor
Comms::Comms(int i2cAddr, CommandCallback callback)
    : I2C_ADDR(i2cAddr), commandCallback(callback) {
    // Set the static pointer to this object instance
    instance = this;
}

// Static wrapper function for Wire.onReceive
void Comms::staticReceiveEvent(int byteCount) {
    if (instance != nullptr) {
        instance->receiveEvent(byteCount);
    }
}

// Actual I2C Receive Event Handler
void Comms::receiveEvent(int byteCount) {
    // Read all incoming bytes into the buffer
    int i = 0;
    while (Wire.available() && i < sizeof(CommandBuffer) - 1) {
        CommandBuffer[i++] = Wire.read();
    }
    CommandBuffer[i] = '\0'; // Null-terminate the string

    // Note: We don't call the callback here because interrupts are often
    // discouraged from doing heavy lifting like printing to Serial.
    i2cCommandReady = true; // Set flag
}

// Initialise Comms module
void Comms::begin() {
    Serial2.begin(115200); // Assuming you are using Serial2 on the ESP32
    Wire.begin(I2C_ADDR);        // Start as I2C Slave
    Wire.onReceive(staticReceiveEvent); // Register the interrupt handler
}

// Called in loop() to process the I2C command once safe (outside of ISR)
void Comms::checkI2C() {
    if (i2cCommandReady) {
        // Execute the command processor using the stored callback
        if (commandCallback) {
            commandCallback(CommandBuffer);
        }

        i2cCommandReady = false; // Reset flag
    }
}

// Called in loop() to check for Serial commands
void Comms::checkSerial() {
    if (Serial2.available() > 0) {
        char incomingByte = Serial2.read();
        
        // Command Terminator check
        if (incomingByte == '\n' || incomingByte == '\r') {
            CommandBuffer[serialBufferIndex] = '\0';
            
            if (serialBufferIndex > 0) {
                Serial.print("Received Serial Command: ");
                Serial.println(CommandBuffer);
                
                // Execute the command processor using the stored callback
                if (commandCallback) {
                    commandCallback(CommandBuffer);
                }
            }
            serialBufferIndex = 0;

        } else {
            // Standard byte processing
            if (serialBufferIndex < MAX_COMMAND_LENGTH - 1) {
                CommandBuffer[serialBufferIndex] = incomingByte;
                serialBufferIndex++;
            } else {
                Serial.println("Serial Command Too Long! Discarding...");
                serialBufferIndex = 0;
            }
        }
    }
}