#define BOUNCE_WITH_PROMPT_DETECTION
#include <Bounce2.h>

#define hat_delay 5     // debounce time delay in milliseconds
#define button_delay 5

#define num_buttons 11  // total number of pushbuttons
#define modifier 8     // index of key in array below to enable keyboard

// define physical button pins
const uint8_t button_pins[num_buttons] = {0,1,2,3,4,5,6,7,8,9,10};
const uint8_t hat_pins[4] = {20,21,22,23};
// assign key presses to ordered array pins
uint16_t keys[8] = {KEY_BACKSPACE,KEY_RETURN,KEY_DELETE,KEY_ESC,KEY_TAB,KEY_LEFT_GUI,KEY_LEFT_ALT,KEY_RIGHT_CTRL};
uint16_t arrows[4] = {KEY_LEFT,KEY_DOWN,KEY_RIGHT,KEY_UP};
Bounce buttons[num_buttons];  // Instantiate and store debounce objects for each pin
Bounce directions[4];

// Save state and record change of hat switches; Map states to x,y for rotation
byte x, y;
uint8_t hat_state[4] = {0,0,0,0};
boolean hat_change = false;
int16_t hat_value_map[4][4] = {
  {-1, 270, 90, -1},
  {0, 315, 45, -1},
  {180, 225, 135, -1},
  {-1, -1, -1, -1}
};

void setup() {
  // Iterate through pins, initialize pins and assign delay
  for (uint8_t i = 0; i < num_buttons; i++) {
    buttons[i].attach(button_pins[i], INPUT_PULLUP);
    buttons[i].interval(button_delay);
  }
  
  for (uint8_t i = 0; i < 4; i++) {
    directions[i].attach(hat_pins[i], INPUT_PULLUP);
    directions[i].interval(hat_delay);
  }

}

void loop() {
  // Update each pin, setting button state high or low from voltage change
  for (uint8_t i = 0; i < num_buttons; i++) {
    buttons[i].update();
    if (buttons[i].fell()) {Joystick.button(i+1, 1);}
    if (buttons[i].rose()) {Joystick.button(i+1, 0);}
  }

  // Update the Bounce objects for the hat switches
  for (uint8_t i = 0; i < 4; i++) {
    directions[i].update();
    if (directions[i].fell()) {hat_state[i] = 1; hat_change = true;}
    if (directions[i].rose()) {hat_state[i] = 0; hat_change = true;}
  }

  // y: U/D states, x: L/R states, mapped to angles listed above
  if (hat_change) {
      // Set the hat value based on the hat switch states
      y = hat_state[3] | (hat_state[1] << 1);
      x = hat_state[0] | (hat_state[2] << 1);
      Joystick.hat(hat_value_map[y][x]);
  }

  if (buttons[modifier].read() == LOW) {
    for (uint8_t i = 0; i < 8; i++) {
      if (buttons[i].fell()) {Keyboard.press(keys[i]);}
      if (buttons[i].rose()) {Keyboard.release(keys[i]);}
    }
    for (uint8_t i = 0; i < 4; i++) {
      if (directions[i].fell()) {Keyboard.press(arrows[i]);}
      if (directions[i].rose()) {Keyboard.release(arrows[i]);}
    }
  }

  // Reset the hat state change flag
  hat_change = false;
}
