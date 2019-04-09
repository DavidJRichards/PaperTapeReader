// paper tape reader test program with OLED display
// djrm 30 March 2019

//#define DEBUG true
#define USEOLED

#include <Wire.h>
#include "GString.h"
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"

#define FEEDHOLE        3
#define NACK            2
#define ACK             4
#define ACK2            LED_BUILTIN
#define DATA0           5
#define DATA1           6
#define DATA2           7
#define DATA3           8
#define DATA4           9
#define DATA5           10
#define DATA6           11
#define DATA7           12

#ifdef DEBUG
#define debugPrintHex(c) Serial.write('<'); Serial.print(c,HEX); Serial.write('>')
#define debugPrint(c) Serial.write(c)
#else
#define debugPrint(c)
#define debugPrintValue(c,i)
#endif

#define ACK_ON  1
#define ACK_OFF 0
#define ACK2_ON  1
#define ACK2_OFF 0
#define NACK_ON  ACK_OFF
#define NACK_OFF ACK_ON

enum States {
  READY,
  BUSY,
  ACK_SET,
  ACK_CLEAR,
  FAULT,
} State;

volatile bool dataReady;
unsigned char pinInputs;

#define INPUT_TYPE INPUT //INPUT_PULLUP

#define I2C_ADDRESS 0x3C
#define RST_PIN -1

#ifdef USEOLED
SSD1306AsciiWire oled;
#endif

char scrbuf[20];

//===================================================================================

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(FEEDHOLE, INPUT/*_PULLUP*/);
  pinMode(ACK, OUTPUT);
  pinMode(ACK2, OUTPUT);
  pinMode(NACK, OUTPUT);
  pinMode(DATA0, INPUT_TYPE);
  pinMode(DATA1, INPUT_TYPE);
  pinMode(DATA2, INPUT_TYPE);
  pinMode(DATA3, INPUT_TYPE);
  pinMode(DATA4, INPUT_TYPE);
  pinMode(DATA5, INPUT_TYPE);
  pinMode(DATA6, INPUT_TYPE);
  pinMode(DATA7, INPUT_TYPE);

  EICRA |= B0001000;    // External Interrupt 1 FALLING
  EIFR  |= B00000010;   // clear interrupt 1 flag - prevents any spurious carry-over
  EIMSK |= B0000010;    // enable Interrupt 1

  State = READY;
  debugPrint('>');

#ifdef USEOLED  
  oled.begin(&Adafruit128x64, I2C_ADDRESS);
  oled.setFont(fixed_bold10x15);
  oled.clear();
  oled.println("OP80a Reader");
  oled.setScrollMode(SCROLL_MODE_AUTO);
#endif
}

//===================================================================================
//       Interrupt Routines

ISR(INT1_vect) {
  
  if(State == READY)
    State = BUSY;  // RDA/strobe signal received by interrupt handler}
  else
    State = FAULT;
}  

//=================================================================================

void loop() {
  static int k=0;

  switch (State) {
    case READY:
      break;
      
    case FAULT:
      Serial.write("Fault");
      State = BUSY;
      break; 
      
    case BUSY: // nStrobe signal received by interrupt handler
      pinInputs = 0U
      | (digitalRead(DATA0) ? 0x01U:0U)
      | (digitalRead(DATA1) ? 0x02U:0U)
      | (digitalRead(DATA2) ? 0x04U:0U)
      | (digitalRead(DATA3) ? 0x08U:0U)
      | (digitalRead(DATA4) ? 0x10U:0U)
      | (digitalRead(DATA5) ? 0x20U:0U)
      | (digitalRead(DATA6) ? 0x40U:0U)
      | (digitalRead(DATA7) ? 0x80u:0U);
      dataReady = 1;
      State = ACK_SET;
      break;
      
    case ACK_SET:
      digitalWrite(ACK, ACK_ON);
      digitalWrite(ACK2, ACK_ON);
      digitalWrite(NACK, NACK_ON);
//      delay(1); //Was set @50. Seems fine @ 1ms
      State = ACK_CLEAR;
      break;
      
    case ACK_CLEAR:
      digitalWrite(ACK, ACK_OFF);
      digitalWrite(ACK2, ACK_OFF);
      digitalWrite(NACK, NACK_OFF);
      State = READY;
      break;

    default:
      Serial.write("State?");
      State = READY;
      break;
  }   
 
  if( dataReady )
  {
    debugPrint(pinInputs);
    
#ifdef DEBUG
    oled.print(k++);
    oled.print(" 0x");
    oled.print(pinInputs,HEX);
    oled.print(" \'");
    oled.write(pinInputs);
    oled.write('\'');
    oled.println();
#endif

#ifdef USEOLED
    sprintf(scrbuf,"%3d 0x%02x",k++,pinInputs);
    if(isprint(pinInputs))
     sprintf(scrbuf+strlen(scrbuf)," \'%c\'",pinInputs);    
    oled.println(scrbuf);
#endif
    
    Serial.write(pinInputs);
    dataReady = 0;
  }
}

// end
