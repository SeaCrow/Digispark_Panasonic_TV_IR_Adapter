/*
 * USB IR Keyboard/Reciver for Panasonic Remote
 * based on Digispark(ATTiny85)
 *
 * Panasonic TV remote signal was found to be consisting of (as seen by ATTiny85 from IR Reciver signal state):
 * Low state(~3,575ms),High state(~1,648ms) header, followed by binary code of 49 bytes.
 * Logical 0 - High State for ~355us,
 * Logical 1 - High State for ~1215us,
 * separated by low state breaks of ~520us.
 *
 *	Default H  | Header LOW HIGH |  0    1      1     0   0    1   etc.
 * . . . ______             _____   _   ____   ____   _   _   ____ . . .
 *             |___________|     |_| |_|    |_|    |_| |_| |_|    
 *
 * By poking for IR LOW state we can detect start of transmission, then by checking the state every RESOLUTION uSeconds,
 * we can count how many times we encountered HIGH state before LOW state. Having that count, lets us recognize if it was
 * Logical 0, Logical 1, Header HIGH or Default HIGH betwen transmissions.
 *
 * 24-10-2018 File Created
 * 27-10-2018 Delay added, poking for transmision every 3ms is enough to catch every transmission start
 * 28-11-2019 Added Definitions for TAB, F4 and changed functions of IR_UP,IR_DOWN old values left commented
 * 27-01-2023 Added Definitions for ENTER, LEFT, RIGHT and added functions to IR_STOP, IR_PAUSE
 * 28-01-2023 Expanded summary.
 *
 */
#include "DigiKeyboard.h"

#define IRpin_PIN PINB	// ATTiny85 had Port B pins
#define IRpin 2			// Signal pin of IR Reciver


#define MAXPULSE		91		// the maximum time we'll listen for in milliseconds (header + 49 logical 1's), serves as timeout.
#define NUMPULSES		49		// max IR pulse pairs to sample
#define RESOLUTION		128		// time between IR measurements in uSeconds

uint64_t irCode = 0;

//Remote button codes
#define IR_POWER	0xD00FCB5
#define IR_DOWN		0xD00ECA5
#define IR_UP		0xD006C25

#define IR_REW		0xD00D2A3
#define IR_PLAY		0xD009061
#define IR_FORW		0xD009263

#define IR_STOP		0xD004011
#define IR_PAUSE	0xD00A071
#define IR_REC		0xD005021

//USB HID CODES
#define KEY_SPACE			44	// youtube/netflix/plex pause/play
#define KEY_J				13	// 10 sec backward in youtube
#define KEY_L				15	// 10 sec forward in youtube
#define KEY_F				9	// full screen in youtube/netflix/plex
#define KEY_UP_ARROW		82
#define KEY_DOWN_ARROW		81
#define KEY_ESC				41
#define KEY_TAB				43
#define KEY_F4				61
#define KEY_ENTER			40
#define KEY_LEFT_ARROW		80	// 10 sec backward in netflix/plex
#define KEY_RIGHT_ARROW		79

void setup()
{
  pinMode(IRpin, INPUT);
  DigiKeyboard.delay(100);
  DigiKeyboard.sendKeyStroke(0);
}

void loop()
{
  irCode = listenForIR(); // Wait for an IR Code
  /*
     if (irCode != 0)    // IR codes detection, open notepad, press remote button and get the code
     {
       DigiKeyboard.sendKeyStroke(0);
       DigiKeyboard.println("iRCode");
       //DigiKeyboard.delay(500);
       uint32_t low = irCode % 0xFFFFFFFF;
       uint32_t high = (irCode >> 32) % 0xFFFFFFFF;
       //DigiKeyboard.print(high, HEX);
       //DigiKeyboard.print("|");
       //DigiKeyboard.delay(500);
       DigiKeyboard.print(low, HEX);
       DigiKeyboard.println(" ");
       irCode = 0;
       DigiKeyboard.delay(500);
     }
   */

  if (irCode != 0)
  {
    uint32_t lowIRcode = irCode % 0xFFFFFFFF;

    switch (lowIRcode) //act as IR keyboard
    {
      case IR_REW:
        DigiKeyboard.sendKeyStroke(KEY_J);
        break;
      case IR_PLAY:
        DigiKeyboard.sendKeyStroke(KEY_SPACE);
        break;
      case IR_FORW:
        DigiKeyboard.sendKeyStroke(KEY_L);
        break;
      case IR_REC:
        DigiKeyboard.sendKeyStroke(KEY_F);
        break;
      case IR_POWER:
        DigiKeyboard.sendKeyStroke(KEY_ESC);
        break;
       case IR_DOWN:
        //DigiKeyboard.sendKeyStroke(KEY_DOWN_ARROW);
        DigiKeyboard.sendKeyPress(0,0); // release all keys
        break;
       case IR_UP:
        //DigiKeyboard.sendKeyStroke(KEY_UP_ARROW);
        DigiKeyboard.sendKeyPress(KEY_TAB,MOD_ALT_LEFT); // press ALT + TAB
        DigiKeyboard.delay(100);
        DigiKeyboard.sendKeyPress(0,MOD_ALT_LEFT); // release TAB but keep ALT pressed
        break;
       case IR_STOP:
		DigiKeyboard.sendKeyStroke(KEY_ENTER);
		break;
       case IR_PAUSE:
		DigiKeyboard.sendKeyStroke(KEY_LEFT_ARROW);
		break;
    }
    DigiKeyboard.delay(100);
    irCode = 0;
  }
  else
  {
    DigiKeyboard.delay(3);
  }

}


uint64_t listenForIR() {  // IR receive code
  uint64_t currentCode = 0;
  int readingNr = 0;
  int pulseNr = 0;
  //DigiKeyboard.update();
  if (IRpin_PIN & _BV(IRpin)) // high state, no data
  {
    return currentCode;
  }
  unsigned long transmissionStart = millis();
  while (1)
  {
    if (IRpin_PIN & _BV(IRpin)) // high state - pulse
    {
      if (pulseNr == NUMPULSES - 1) // we finished code, are now in high state between transmissions
      {
        return currentCode;
      }
      readingNr++;
    }

    if (! (IRpin_PIN & _BV(IRpin))) // low state - pulse break
    {
      if (readingNr != 0) // did we just finished reading pulse?
      {
        if (readingNr > 1 && readingNr < 5) // it was logical 0
        {
          currentCode = currentCode << 1;
          currentCode |= 0;
          pulseNr++;
        }
        if (readingNr > 8 && readingNr < 12) // it was logical 1
        {
          currentCode = currentCode << 1;
          currentCode |= 1;
          pulseNr++;
        }
        if (readingNr > 12 && readingNr < 15) // it was second part of header
        {
          pulseNr = 0;
        }
        //reading number >= 13 - second part of header, disregard that
        readingNr = 0;
      }
    }

    if ((millis() - transmissionStart) > MAXPULSE) // we messed up, missed bit(s) and are now past the transmission
    {
      return 0;
    }

    delayMicroseconds(RESOLUTION);
  }
}
