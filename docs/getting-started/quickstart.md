First step is to check the [Kit contents](kit_contents.md) to make sure nothing is missing, and to familiarise yourself with the terminology used.

# Wiring Up

The headers on the main breakout board are all labelled to show what they connect to, and there is a label at the end to show which row of pins is what

* S - Signal/Data
* V - Voltage (5v)
* G - Ground

Make sure that the S,V, and G pins are connected to the correct pins on the light boards. 

# Connections

## Lights

* LADD - Main chopper ladder light
* PERI - RGB Periscope light
* AUX1-4 - For future use

## Serial

There are two serial ports broken out for use. Serial2 by default is configured to listen for commands. See [Commands](advanced/commands.md).

## i2c

You can connect a controller and communicate over i2c. See [Commands](advanced/commands.md).

## PT

This can be used to accept an output from something like a pololu maestro to turn on the periscope LED if connected. It accepts a 5v signal on the S pin, and make sure to connect a ground from the maestro to the G pin.

## RC

This enables/disables the trim pots. When you put a jumper on these pins, the trim pots will be disabled and you can send commands over serial or i2c to control the lights. Boot settings will be saved to EEPROM, so on rebooting the lights will return to their last setting.

To set the default boot setting before sending the commands, use the trim pots without the jumper in place, and then once set to your liking, place the jumper on the pins whilst the board is still powered. 

# First Power

For testing, simply plug the ESP32 board into any USB socket. If powering from the screw terminals make sure it is a clean 5V supply. (ie, don't use a bunch of AA batteries to 'approximate' 5V)

{% hint style="warning" %}
Whilst it is fine to use the USB connector for initial testing, they are fragile so shouldn't be used in a final install.
{% endhint %}

For more detailed information on powering the lights check the [Power](power.md) page

# Trim pots

There are three trim pots to tune the lights. These only work if no jumper is present on the RC pins. The trim pots are:

* Speed - Sets the speed the ladder light moves up and down
* Bri - Sets the brightness of the moving light
* Bck - Sets the background brightness of the light board. 

