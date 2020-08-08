// Arduino is too slow to keep up with DRAM RAS-to-CAS delay, but using
// registers it is possible to come at least as close as possible to get
// the DRAM working. Settinig address is absolutely critical operation and
// needs to be as fast as possible. To make this possible address must be
// set in one go as coherent register. This can be tricky to get all address
// bits in one register on some Arduinos, however NANO gives us with A0-A7
// as one register a perfect fit. So we have WE,OE,RAS,CAS in one register,
// D0-D3 in another and A0-A7 in a third one. This way we can set the signals
// as fast as possible. However, with some other models this can work as well.

// Pins configuration for Arduino NANO:
// WE = Pin D2
// OE = Pin D3
// RAS = Pin D4
// CAS = Pin D5
// D0-3 = Pin D8-11
// A0-7 = Pin A0-7

enum Mask {
  WE   = B00000100,
  OE   = B00001000,     
  RAS  = B00010000,  
  CAS  = B00100000,
  DATA = B00001111,
  ADDR = B11111111,
  CTRL = WE | OE | RAS | CAS, 
};

void setup() {
  Serial.begin(115200);
  Serial.println("4464 DRAM Tester");

  DDRB |= DATA;
  DDRC |= ADDR;
  DDRD |= CTRL;
  
  PORTD = CTRL;
  for (auto i = 0; i < 16; i++) {
    PORTD ^= RAS;
  }
}

void writeData(word addr, byte data) {
  DDRB |= DATA;
  PORTB = (PORTB & ~DATA) | (data & DATA);
  PORTC = addr & ADDR;
  PORTD ^= RAS;
  PORTC = (addr >> 8) & ADDR;
  PORTD ^= CAS;
  PORTD ^= WE;  
  PORTD ^= WE;
  PORTD ^= CAS;
  PORTD ^= RAS;
}

byte readData(word addr) {
  DDRB &= ~DATA;
  PORTC = addr & ADDR;
  PORTD ^= RAS;
  PORTC = (addr >> 8) & ADDR;
  PORTD ^= CAS;
  PORTD ^= OE;
  PORTD ^= OE;
  byte data = PINB & DATA;    
  PORTD ^= CAS;
  PORTD ^= RAS;  
  return data;
}

void simple() {
  static const size_t elements = 64;  
  for (size_t i = 0; i < elements; i++) {
    writeData(i, i & 0xF);
  }
  byte output[elements];
  for (size_t i = 0; i < elements; i++) {
    output[i] = readData(i);
  }
  
  for (size_t i = 0; i < elements; i++) {
    Serial.print(i, BIN);
    Serial.print(" => ");    
    Serial.println(output[i], BIN);
    if ((i & 0xF) != output[i]) {
        Serial.println("Bad!!!");
        return;
    }
  }
  Serial.println("Good!!!");
}

void test(byte pattern) {
  Serial.print("Test pattern ");
  Serial.print(pattern, BIN);
  Serial.print("... ");
  for (auto c = 0; c < 256; c++) {
    for (auto r = 0; r < 256; r++) {
      auto addr = (c << 8) | r;
      writeData(addr, pattern); 
    }
  }
  for (auto c = 0; c < 256; c++) {
    for (auto r = 0; r < 256; r++) {
      auto addr = (c << 8) | r;
      auto result = readData(addr);
      if (result != pattern) {        
        Serial.print("Failed @");
        Serial.print(addr, HEX);
        Serial.print("=");
        Serial.println(result, BIN);
        return;
      }   
    }    
  }  
  Serial.println("OK");
}

static bool first = true;

void loop() {
  if (first) {    
    first = false;
    simple();
    //test(B0000);
    //test(B1111);
  }
}
