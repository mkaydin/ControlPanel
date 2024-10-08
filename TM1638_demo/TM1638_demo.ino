
#include "wiring_shift_mod.h"

#define CLOCK_TYPE CLOCK_INVERT
#define CLOCK_DELAY_US 1

const int strobe_pin =  4;
const int clock_pin  = 16;
const int data_pin   = 17;

#define COUNTING_MODE 0
#define SCROLL_MODE 1
#define BUTTON_MODE 2

void setup()
{
  pinMode(strobe_pin, OUTPUT);
  pinMode(clock_pin, OUTPUT);
  pinMode(data_pin, OUTPUT);

  sendCommand(0x8f);  // activate
  reset();
}

void loop()
{
  static uint8_t mode = SCROLL_MODE;

  switch(mode)
  {
  case SCROLL_MODE:
    mode += scroll();
    break;
  case BUTTON_MODE:
    buttons();
    break;
  }

  delay(200);
}

void sendCommand(uint8_t value)
{
  digitalWrite(strobe_pin, LOW);
  shiftOutMod(data_pin, clock_pin, LSBFIRST, CLOCK_TYPE, CLOCK_DELAY_US, value);
  digitalWrite(strobe_pin, HIGH);
}

void reset()
{
  sendCommand(0x40); // set auto increment mode
  digitalWrite(strobe_pin, LOW);
  shiftOutMod(data_pin, clock_pin, LSBFIRST, CLOCK_TYPE, CLOCK_DELAY_US, 0xc0);   // set starting address to 0
  for(uint8_t i = 0; i < 16; i++)
  {
    shiftOutMod(data_pin, clock_pin, LSBFIRST, CLOCK_TYPE, CLOCK_DELAY_US, 0x00);
  }
  digitalWrite(strobe_pin, HIGH);
}

bool scroll()
{
  
  uint8_t scrollText[] =
  {
    /* */ /* */ /* */ /* */ /* */ /* */ /* */ /* */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /*C*/ /*O*/ /*N*/ /*S*/ /*O*/ /*L*/ /*E*/ /* */
    0x39, 0x3F, 0x37, 0x6D, 0x3F, 0x38, 0x79, 0x00,
    /* */ /* */ /* */ /* */ /* */ /* */ /* */ /* */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /*C*/ /*O*/ /*N*/ /*S*/ /*O*/ /*L*/ /*E*/ /* */
    0x39, 0x3F, 0x37, 0x6D, 0x3F, 0x38, 0x79, 0x00,
  };
  

  static uint8_t index = 0;
  uint8_t scrollLength = sizeof(scrollText);

  sendCommand(0x40);
  digitalWrite(strobe_pin, LOW);
  shiftOutMod(data_pin, clock_pin, LSBFIRST, CLOCK_TYPE, CLOCK_DELAY_US, 0xc0);

  for(int i = 0; i < 8; i++)
  {
    uint8_t c = scrollText[(index + i) % scrollLength];

    shiftOutMod(data_pin, clock_pin, LSBFIRST, CLOCK_TYPE, CLOCK_DELAY_US, c);
    shiftOutMod(data_pin, clock_pin, LSBFIRST, CLOCK_TYPE, CLOCK_DELAY_US, c != 0 ? 1 : 0);
  }

  digitalWrite(strobe_pin, HIGH);

  index = ++index % (scrollLength << 1);

  return index == 0;
}

void buttons()
{
  uint8_t promptText[] =
  {
    /* */ /* */ /* */ /* */ /* */ /* */ /* */ /* */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /*C*/ /*O*/ /*N*/ /*S*/ /*O*/ /*L*/ /*E*/ /* */
    0x39, 0x3F, 0x37, 0x6D, 0x3F, 0x38, 0x79, 0x00,
    /* */ /* */ /* */ /* */ /* */ /* */ /* */ /* */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /*C*/ /*O*/ /*N*/ /*S*/ /*O*/ /*L*/ /*E*/ /* */
    0x39, 0x3F, 0x37, 0x6D, 0x3F, 0x38, 0x79, 0x00,
  };

  static uint8_t block = 0;

  uint8_t textStartPos = (block / 4) << 3;
  for(uint8_t position = 0; position < 8; position++)
  {
    sendCommand(0x44);
    digitalWrite(strobe_pin, LOW);
    shiftOutMod(data_pin, clock_pin, LSBFIRST, CLOCK_TYPE, CLOCK_DELAY_US, 0xC0 + (position << 1));
    shiftOutMod(data_pin, clock_pin, LSBFIRST, CLOCK_TYPE, CLOCK_DELAY_US, promptText[textStartPos + position]);
    digitalWrite(strobe_pin, HIGH);
  }

  block = (block + 1) % 16;

  uint8_t buttons = readButtons();

  for(uint8_t position = 0; position < 8; position++)
  {
    uint8_t mask = 0x1 << position;

    setLed(buttons & mask ? 1 : 0, position);
  }
}

uint8_t readButtons(void)
{
  uint8_t buttons = 0;
  digitalWrite(strobe_pin, LOW);
  shiftOutMod(data_pin, clock_pin, LSBFIRST, CLOCK_TYPE, CLOCK_DELAY_US, 0x42);

  pinMode(data_pin, INPUT);

  for (uint8_t i = 0; i < 4; i++)
  {
    uint8_t v = shiftInMod(data_pin, clock_pin, LSBFIRST, CLOCK_TYPE, CLOCK_DELAY_US) << i;
    buttons |= v;
  }

  pinMode(data_pin, OUTPUT);
  digitalWrite(strobe_pin, HIGH);
  return buttons;
}

void setLed(uint8_t value, uint8_t position)
{
  pinMode(data_pin, OUTPUT);

  sendCommand(0x44);
  digitalWrite(strobe_pin, LOW);
  shiftOutMod(data_pin, clock_pin, LSBFIRST, CLOCK_TYPE, CLOCK_DELAY_US, 0xC1 + (position << 1));
  shiftOutMod(data_pin, clock_pin, LSBFIRST, CLOCK_TYPE, CLOCK_DELAY_US, value);
  digitalWrite(strobe_pin, HIGH);
}
