# Coding Environment

## IDE

Download and set up the [Arduino IDE](https://www.arduino.cc/en/software/)

Once this is done you will need to install some libraries. 

* adafruit/Adafruit NeoMatrix@^1.3.3
* adafruit/Adafruit NeoPixel@^1.15.1
* adafruit/Adafruit GFX Library@^1.12.1

The adafruit libraries can be installed as per a standard module in the IDE.


## Installing the ESP32 board library.

As the lights run on an ESP32 and that board is not in the default installation, you need to add a board library. 

Go to File->Preferences and add this to the box labelled 'Additional Boards Manager URL'

```https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json```

Then you can go to Tools->Board->Board Manager 

This will open up a window from which you can install the ESP32 library. Enter ESP32 in the search bar and you should see an entry by Espressif Systems available. 



