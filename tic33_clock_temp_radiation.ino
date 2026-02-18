const int LOAD = 3; const int DIN = 4; const int LCLK = 5; const int DCLK = 6;

const uint8_t font[] = {
  0x5F, 0x50, 0x6D, 0x79, 0x72, 0x3B, 0x3F, 0x51, 0x7F, 0x7B, // 0-9 [0-9]
  0x77, 0x3E, 0x0F, 0x7C, 0x2F, 0x27, 0x1F, 0x76, 0x36, 0x10, // A, b, C, d, E, F, G, H, h, i
  0x58, 0x0E, 0x34, 0x5F, 0x3C, 0x67, 0x73, 0x24, 0x3B, 0x2E, // J, L, n, O, o, P, q, r, S, t
  0x1C, 0x7A, 0x20, 0x08, 0x63, 0x80, 0x00                          // u, Y, -, _, °, ., пробел
};

uint8_t getChar(char c) {
  if (c >= '0' && c <= '9') return font[c - '0'];
  switch (c) {
    case 'A': case 'a': return font[10]; case 'b': return font[11];
    case 'C': case 'c': return font[12]; case 'd': return font[13];
    case 'E': case 'e': return font[14]; case 'F': case 'f': return font[15];
    case 'G': case 'g': return font[16]; case 'H': return font[17];
    case 'h': return font[18]; case 'i': return font[19];
    case 'J': case 'j': return font[20]; case 'L': case 'l': return font[21];
    case 'n': return font[22]; case 'O': return font[23];
    case 'o': return font[24]; case 'P': case 'p': return font[25];
    case 'q': return font[26]; case 'r': return font[27];
    case 'S': case 's': return font[28]; case 't': return font[29];
    case 'u': return font[30]; case 'Y': case 'y': return font[31];
    case '-': return font[32]; case '_': return font[33];
    case '*': return font[34]; case '.': return font[35];
    default: return font[36];
  }
}

void sendByte(uint8_t b) {
  for (int i = 0; i < 8; i++) {
    digitalWrite(DIN, (b >> i) & 0x01);
    digitalWrite(DCLK, HIGH); delayMicroseconds(10);
    digitalWrite(DCLK, LOW); delayMicroseconds(10);
  }
}

// УЛУЧШЕННАЯ ФУНКЦИЯ: обрабатывает точку без занятия лишнего разряда
void printText(String msg) {
  uint8_t buf[9] = {0};
  int bufPos = 0;

  for (int i = 0; i < msg.length() && bufPos < 9; i++) {
    if (msg[i] == '.' && bufPos > 0) {
      buf[bufPos - 1] |= 0x80; // Склеиваем точку с предыдущим символом
    } else {
      buf[bufPos] = getChar(msg[i]);
      bufPos++;
    }
  }

  digitalWrite(LOAD, HIGH); delayMicroseconds(10); digitalWrite(LOAD, LOW);
  for (int i = 0; i < 9; i++) sendByte(buf[i]);
  digitalWrite(LOAD, HIGH); delayMicroseconds(10); digitalWrite(LOAD, LOW);
}

void setup() {
  pinMode(LOAD, OUTPUT); pinMode(DIN, OUTPUT);
  pinMode(LCLK, OUTPUT); pinMode(DCLK, OUTPUT);
}

int screenMode = 0;
unsigned long lastSwitch = 0;

void loop() {
  // Фаза LCLK
  static unsigned long lp = 0;
  if (millis() - lp >= 10) { digitalWrite(LCLK, !digitalRead(LCLK)); lp = millis(); }

  // ПЕРЕКЛЮЧЕНИЕ РАЗ В 2 СЕКУНДЫ (2000 мс)
  if (millis() - lastSwitch > 2000) {
    screenMode = (screenMode + 1) % 3;
    lastSwitch = millis();
  }

  static unsigned long lastRefresh = 0;
  if (millis() - lastRefresh > 500) {
    lastRefresh = millis();
    
    if (screenMode == 0) {
      printText(" 12-30-45"); 
    } 
    else if (screenMode == 1) {
      // ТОЧКА ТЕПЕРЬ СКЛЕЕНА: "25.6" займет 3 разряда, а не 4
      printText(" t 25.6* C"); 
    } 
    else if (screenMode == 2) {
      // СМЕЩЕНИЕ РАДИАЦИИ: убрали один пробел слева
      printText(" rAd  14 "); 
    }
  }
}
