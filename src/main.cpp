#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

#define SW  4
//up pin
#define DT  3
//down pin
#define CLK 2

#define KEY 5
#define GND 6

U8G2_SH1106_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
static const unsigned char volume_high[] U8X8_PROGMEM = {0x98,0x00,0x1c,0x01,0x57,0x01,0x51,0x01,0x57,0x01,0x1c,0x01,0x98,0x00};
static const unsigned char volume_low[] U8X8_PROGMEM = {0x18,0x1c,0x57,0x51,0x57,0x1c,0x18};
static const unsigned char volume_mute[] U8X8_PROGMEM = {0x18,0x00,0x5c,0x04,0x97,0x02,0x11,0x01,0x97,0x02,0x5c,0x04,0x18,0x00};
static const unsigned char button_unselected[] U8X8_PROGMEM = {0xfc,0xff,0x01,0x02,0x00,0x02,0x01,0x00,0x04,0x01,0x00,0x04,0x81,0x0f,0x04,0x41,0x10,0x04,0x21,0x20,0x04,0x11,0x40,0x04,0x11,0x40,0x04,0x11,0x40,0x04,0x11,0x40,0x04,0x11,0x40,0x04,0x21,0x20,0x04,0x41,0x10,0x04,0x81,0x0f,0x04,0x01,0x00,0x04,0x01,0x00,0x04,0x03,0x00,0x06,0xfe,0xff,0x03,0xfc,0xff,0x01};
static const unsigned char button_selected[] U8X8_PROGMEM = {0xfc,0xff,0x01,0xfe,0xff,0x03,0xff,0xff,0x07,0xff,0xff,0x07,0x7f,0xf0,0x07,0xbf,0xef,0x07,0xdf,0xdf,0x07,0xef,0xbf,0x07,0xef,0xbf,0x07,0xef,0xbf,0x07,0xef,0xbf,0x07,0xef,0xbf,0x07,0xdf,0xdf,0x07,0xbf,0xef,0x07,0x7f,0xf0,0x07,0xff,0xff,0x07,0xff,0xff,0x07,0xff,0xff,0x07,0xfe,0xff,0x03,0xfc,0xff,0x01};

struct Button {
  bool pressed = false;
  bool press = false;
  String action;
} buttons[5];

int prev;
bool buttonPressed = false;
int buttonToChange = 0;
bool needsRedraw = true;

void buttonPress(int pin, Button &button) {
  button.press = !(bool)digitalRead(pin);
  if (button.press && !button.pressed) {
      Serial.println(button.action);
  }
  button.pressed = button.press;
}

void knobPress(){
  buttons[SW].press = !(bool)digitalRead(SW);
  if (buttons[SW].press && !buttons[SW].pressed) {
    delay(250);
    if (digitalRead(SW) == LOW) {
      buttonPressed = true;
      needsRedraw = true;
    } else {
      Serial.println(buttons[SW].action);
      buttonPressed = false;
      needsRedraw = true;
    }
  } else if (!buttons[SW].press && buttonPressed) {
    needsRedraw = true;
    buttonPressed = false;
  }

  buttons[SW].pressed = buttons[SW].press;
}

void rotateLeft(){
  if (buttons[SW].press) {
    buttonToChange = (buttonToChange + 1) % 4;
    needsRedraw = true;
  } else {
    Serial.println("0VUP0");
  }
}
void rotateRight(){
  if (buttons[SW].press) {
    buttonToChange = (buttonToChange - 1 + 4) % 4;
    needsRedraw = true;
  } else {
    Serial.println("0VDW0");
  }
}

void buttonPress(){
  Serial.println("0VMT0");
  needsRedraw = true;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  pinMode(SW, INPUT_PULLUP);
  //setup buttons
  for (int i = 0; i <= 6; i += 2) {
    pinMode(KEY + i, INPUT_PULLUP);
    pinMode(GND + i, OUTPUT);
    digitalWrite(GND + i, LOW);
    buttons[i / 2].action = "test";
  }
  buttons[SW].action = "0VMT0";

  prev = digitalRead(CLK);
  u8g2.begin();
}

void drawScreen() {
    u8g2.setFontMode(1);
    u8g2.setBitmapMode(1);

    u8g2.firstPage();
    do {
      // line at the top
      u8g2.drawLine(0, 11, 127, 11);

      // Time
      u8g2.setFont(u8g2_font_5x8_tr);
      u8g2.drawStr(2, 8, "10:00");

      // Date
      u8g2.drawStr(85, 8, "03.06.26");
      // Volume icons
      // u8g2.drawXBMP(56, 2, 11, 7, volume_mute);
      // u8g2.drawXBMP(56, 2, 7, 7, volume_low);
      u8g2.drawXBMP(56, 2, 9, 7, volume_high);

      for (int i = 0; i < 4; i++) {
        //Buttons
        if (buttonPressed && buttonToChange == i) {
          u8g2.drawXBMP((10 + i * 30), 25, 19, 20, button_selected);
        } else {
          u8g2.drawXBMP((10 + i * 30), 25, 19, 20, button_unselected);
        }
      }

      if (buttonPressed) {
        // Text instructions
        u8g2.setFont(u8g2_font_6x10_tr);
        u8g2.drawStr(14, 21, "Select the button");
        u8g2.drawStr(11, 58, "Press the shortcut");
      }
  } while (u8g2.nextPage());
}

void loop() {
  // put your main code here, to run repeatedly:
  int rotated = 0;
  int curr = digitalRead(CLK);
  if (prev != curr) {
    if (digitalRead(DT) != curr) {
      rotated = -1;
    } else {
      rotated = 1;
    }
  }
  prev = curr;

  if (rotated == -1) {
    rotateLeft();
  } else if (rotated == 1) {
    rotateRight();
  }

  for (int i = 0; i <= 6; i += 2) {
    buttonPress(KEY + i, buttons[i / 2]);
  }
  knobPress();

  if (needsRedraw) {
    drawScreen();
    needsRedraw = false;
  } 

}