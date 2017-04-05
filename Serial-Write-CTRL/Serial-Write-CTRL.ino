
#include "EEPROM.h"

#define SW  0
#define RXD 2
#define DTR 1
#define ISP 3
#define RESET 4


volatile boolean isp = false;
volatile boolean wait_rxd = false;
volatile static  byte after_rst = 0;
volatile static  byte rts_puls = 0;

boolean chk_status(byte sig, unsigned int wt, boolean st) {
  unsigned int n = 0;
  while (n++ < wt && digitalRead(sig) == st ) {
    if (sig != SW && digitalRead(SW) == LOW) break;
    delay(1);
  }
  return n >= wt;
}

void set_mode() {
//  int modeval;
//  modeval = (digitalRead(RXD));
  EEPROM.write(0, digitalRead(RXD));
  EEPROM.write(1, digitalRead(DTR));
//  byte ss = LOW;
//  if (modeval == 0) modeval = 1000;
//  else modeval = 100;
//  while (true) {
//    digitalWrite(ISP, ss);
//    delay(modeval);
//    ss = !ss;
 // }
}

#define set_isp() stb(LOW)
#define do_reset() stb(HIGH)
void stb(byte sts) {
  digitalWrite(RESET, HIGH);
  digitalWrite(ISP, sts);
  delay(2); digitalWrite(RESET, LOW);
  delay(2); digitalWrite(RESET, HIGH);
  if (sts == LOW) wait_rxd = true;
  else wait_rxd = false;
  isp = false;
}

void no_isp() {
  digitalWrite(ISP, HIGH);
  isp = false;
  wait_rxd = false;
}

void do_isp() {
  delay(50);
  if (digitalRead(DTR) == LOW && !isp && !wait_rxd ) {
    delay(200);
    if ((digitalRead(DTR) == HIGH) || rts_puls) {
      set_isp();
    }
  }
}


void setup() {
  //  isp = false;
  //  wait_rxd = false;
  pinMode(SW, INPUT_PULLUP);
  pinMode(DTR, INPUT);
  pinMode(RXD, INPUT);
  pinMode(ISP, OUTPUT);
  digitalWrite(ISP,HIGH);
  if (chk_status(SW, 8000, LOW)) set_mode();
  after_rst = EEPROM.read(0);
  rts_puls = EEPROM.read(1);
  pinMode(RESET, OUTPUT);
  do_reset();
  attachInterrupt(0, do_isp, FALLING);
}

void loop() {
  if (digitalRead(SW) == LOW) {
    //      no_isp();
    if (chk_status(SW, 1000, LOW))set_isp();
    else do_reset();
    while (digitalRead(SW) == LOW);
  }

  if (wait_rxd) {
    if (digitalRead(RXD) == LOW) {
      isp = true;
      wait_rxd = false;
    }
  }
  if (chk_status(RXD, 1500, HIGH) && isp) {
    //      no_isp();
    if (after_rst == 1) do_reset();
    else no_isp();
  }
}
