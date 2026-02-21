#include <Arduino.h>

#define SW  4
//up pin
#define DT  3
//down pin
#define CLK 2

#define KEY 5
#define GND 6

struct Button {
  bool pressed = false;
  bool press = false;
  String action;
} buttons[5];

int prev;
bool buttonPressed = false;

void buttonPress(int pin, Button &button) {
  button.press = !(bool)digitalRead(pin);
  if (button.press && !button.pressed) {
      Serial.println(button.action);
  }
  button.pressed = button.press;
}

void knobPress(){
  buttons[SW].press = !(bool)digitalRead(SW);
  if (!buttons[SW].press && buttons[SW].pressed) {
      if (buttonPressed) {
        buttonPressed = false;
      } else {
        Serial.println(buttons[SW].action);
      }
  }
  buttons[SW].pressed = buttons[SW].press;
}


void rotateLeft(){
  if (buttons[SW].press) {
    Serial.println("holding");
    buttonPressed = true;
  } else {
    Serial.println("0VUP0");
  }
}

void rotateRight(){
  if (buttons[SW].press) {
    Serial.println("holding");
    buttonPressed = true;
  } else {
    Serial.println("0VDW0");
  }
}
void buttonPress(){
  Serial.println("0VMT0");
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  pinMode(SW, INPUT_PULLUP);
  for (int i = 0; i <= 6; i += 2) {
    pinMode(KEY + i, INPUT_PULLUP);
    pinMode(GND + i, OUTPUT);
    digitalWrite(GND + i, LOW);
    buttons[i / 2].action = "test";
  }
  buttons[SW].action = "0VMT0";
  prev = digitalRead(CLK);
}

void loop() {
  // put your main code here, to run repeatedly:
  int curr = digitalRead(CLK);
  if (prev != curr) {
    if (digitalRead(DT) != curr) {
      rotateLeft();
    } else {
      rotateRight();
    }
  }
  prev = curr;

  for (int i = 0; i <= 6; i += 2) {
    buttonPress(KEY + i, buttons[i / 2]);
  }
  knobPress();

  delay(5);
}