First step is to check the [Kit contents](kit_contents.md) to make sure nothing is missing, and to familiarise yourself with the terminology used.

# Wiring Up

The headers on the main breakout board are all labelled to show what they connect to, and there is a label at the end to show which row of pins is what

* S - Signal/Data
* V - Voltage (5v)
* G - Ground

Make sure that the S,V, and G pins are connected to the correct pins on the light boards. 

# First Power

For testing, simply plug the ESP32 board into any USB socket. If powering from the screw terminals make sure it is a clean 5V supply. (ie, don't use a bunch of AA batteries to 'approximate' 5V)

{% hint style="warning" %}
Whilst it is fine to use the USB connector for initial testing, they are fragile so shouldn't be used in a final install.
{% endhint %}

For more detailed information on powering the Astropixels check the [Power](power.md) page
