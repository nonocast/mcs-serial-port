#include <8052.h>

#define true 1
#define false 0

#define low 0;
#define high 1;

typedef int BOOL;
typedef unsigned char BYTE;
typedef unsigned int WORD;

void delay(unsigned int);
void init();
void initUart();
void initLights();
void sendByte(BYTE);
void sendString(char*);
BOOL scanButtonClick(int);
void scanButtonsClick();

BOOL busy = false;

void loop();

void main() {
  init();
  loop();
}

void init() {
  initLights();
  initUart();
  sendString("hello world\n");
}

void loop() {
  while (true) {
    scanButtonsClick();
  }
}

void uart() __interrupt(4) {
  if (RI) {
    RI = false;  // clear receive interrupt flag
  }
  if (TI) {
    TI = false;    // clear transmit interrupt flag
    busy = false;  // clear transmit busy flag
  }
}

void initLights() {
  P1 = 0xff;
}

void initUart() {
  SCON = 0x50;       //0b 0101 0000
  TMOD = 0x20;       // set timer1 as 8-bit auto reload mode
  TH1 = TL1 = 0xfd;  // means 9600
  TR1 = 1;           // timer1 start run
  ES = 1;            // enable uart interrupt
  EA = 1;            // open master inerrupt switch
}

void sendByte(BYTE data) {
  while (busy) {}
  busy = true;
  SBUF = data;
}

void sendString(char* string) {
  while (*string) {
    sendByte(*string++);
  }
}

BOOL scanButtonClick(int i) {
  BOOL result = false;

  int mask = 0x01 << i;
  if ((P3 & mask) == 0) {
    delay(10);
    if ((P3 & mask) == 0) {
      while ((P3 & mask) == 0) {
        P1_0 = 0;
      }
      P1_0 = 1;
      result = true;
    }
  }
  return result;
}

void scanButtonsClick() {
  if(scanButtonClick(4)) {
    sendString("click: button 1\n");
  }
  if(scanButtonClick(5)) {
    sendString("click: button 2\n");
  }
  if(scanButtonClick(6)) {
    sendString("click: button 3\n");
  }
  if(scanButtonClick(7)) {
    sendString("click: button 4\n");
  }
}

void delay(unsigned int i) {
  unsigned char j;
  for (i; i > 0; i--)
    for (j = 100; j > 0; j--)
      ;
}