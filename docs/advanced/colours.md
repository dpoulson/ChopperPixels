# Adding more colours

Currently there are 6 colours:

```
#define NUM_COLOURS 6

const uint16_t colours[NUM_COLOURS][3] = {
  {255,0,0}, // Red
  {0,255,0}, // Green
  {0,0,255},  // Blue
  {255,255,0}, // Yellow
  {255,0,255}, // Magenta
  {0,255,255}, // Cyan
};
```

You can select the colour you want by just sending the command: 

```COLOUR=<index>```

where index is the number of the colour in the array. (1 for red, 2 for green, etc.)

If you want to add more custom colours, just add them to the `colours` array and increase the `NUM_COLOURS` constant.


