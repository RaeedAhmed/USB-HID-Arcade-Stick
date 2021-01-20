
#include <Bounce.h>
#include <vector>

// ============================================================================
//                                  Config
// ============================================================================

// Define the debounce delay values (ms) for the hat and button switches
#define hat_delay 5
#define button_delay 5

// Define number of pushbuttons
#define num_buttons 11
// Starting pin of four hat pins (Change pins in if(hat_change{}))
#define hat_start 20
// Select button pin to enable Keyboard/Mouse while pressed
// Comment out definition if only using joystick
#define modifier 8
#define lclick 9 // mouse click button
#define rclick 10
uint16_t keys[8] = {KEY_LEFT,KEY_DOWN,KEY_UP,KEY_RIGHT,KEY_ESC,KEY_LEFT_GUI,KEY_BACKSPACE,KEY_RETURN};

// ============================================================================
//                                End Config
// ============================================================================

// Store Bounce objects in vector
std::vector<Bounce> buttons;
std::vector<Bounce> directions;

// Save state and record change of hat switches; Map states to x,y for rotation
byte x, y;
int8_t v,h;
uint8_t hat_state[4] = {0,0,0,0};
boolean hat_change = false;
int16_t hat_value_map[4][4] = {
  {-1, 270, 90, -1},
  {0, 315, 45, -1},
  {180, 225, 135, -1},
  {-1, -1, -1, -1}
};

// Cursor pos x: right, pos y: down
int8_t cursor[4][4][2] = {
  {{0,0}, {0,10}, {0,-10}, {0,0}},
  {{10,0}, {7,7}, {7,-7}, {0,0}},
  {{-10,0}, {-7,7}, {-7,-7}, {0,0}},
  {{0,0}, {0,0}, {0,0}, {0,0}}
};

void setup() {
  // Debounce objects for buttons and hat switches, initialize pins
  buttons.reserve(num_buttons);
  for (uint8_t i = 0; i < num_buttons; i++) {
    buttons.push_back(Bounce(i, button_delay));
    pinMode(i, INPUT_PULLUP);
  }
  
  directions.reserve(4);
  for (uint8_t i = hat_start; i < hat_start + 4; i++) {
    directions.push_back(Bounce(i, hat_delay));
    pinMode(i, INPUT_PULLUP);
  }
}

void loop() {
  // Update button Bounce objects, send/ stop button 
  for (uint8_t i = 0; i < num_buttons; i++) {
    buttons[i].update();
    if (buttons[i].fallingEdge()) {Joystick.button(i+1, 1);}
    if (buttons[i].risingEdge()) {Joystick.button(i+1, 0);}
  }

  // Update the Bounce objects for the hat switches
  for (uint8_t i = 0; i < 4; i++) {
    directions[i].update();
    if (directions[i].fallingEdge()) {hat_state[i] = 1; hat_change = true;}
    if (directions[i].risingEdge()) {hat_state[i] = 0; hat_change = true;}
  }

  // y: U/D states, x: L/R states, mapped to angles listed above
  if (hat_change) {
      // Set the hat value based on the hat switch states
      y = hat_state[3] | (hat_state[1] << 1);
      x = hat_state[0] | (hat_state[2] << 1);
      Joystick.hat(hat_value_map[y][x]);
  }

  #ifdef modifier
  if (buttons[modifier].read() == LOW) {
    for (uint8_t i = 0; i < 8; i++) {
      if (buttons[i].fallingEdge()) {Keyboard.press(keys[i]);Keyboard.release(keys[i]);}
    }
    if (buttons[lclick].fallingEdge()) {Mouse.click();}
    if (buttons[rclick].fallingEdge()) {Mouse.click(MOUSE_RIGHT);} 
    h = cursor[y][x][0];
    v = cursor[y][x][1];
    Mouse.move(h,v);
    delay(12);
  }
  #endif

  // Reset the hat state change flag
  hat_change = false;
}
