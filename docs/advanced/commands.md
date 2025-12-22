# Available Commands

Both Serial and i2c use the same command structure. 

Speed, brightness, and back brightness are only enabled for remote control when the RC jumper is in place. If a command is sent to adjust these values without the jumper in place, they will instantly revert back to the value of set by the trim pot. 

Placing the RC jumper in place whilst running will save the current trim pot values to EEPROM. Much like the SAVE command will. On boot, the EEPROM values will be loaded. 

Command structure is:

```<com>=<value>```

This is still a Work In Progress. 

If anyone has ideas, let me know. 

## Current Commands:

### Effect

Switch between different effects with: 

```EFFECT=<num>```

where ```<num>``` is one of: 

* 0 - Larson Scanner (default)
* 1 - VU Meter
* 2 - Reverse VU Meter
* 3 - Center Out VU Meter
* 4 - Random

### Speed

Adjust the speed with:

```SPEED=<num>```

Where ```<num>``` is between 0 and 100. This is the delay between each animation loop, so the lower the number the faster it is.

### Colour

Set the colour index from 0 to 6 (Red, Green, Blue) with:

```COLOUR=<num>```

### Save

You can save the settings to eeprom, so that if in RC mode they are restored when you power up again. 

```SAVE```

