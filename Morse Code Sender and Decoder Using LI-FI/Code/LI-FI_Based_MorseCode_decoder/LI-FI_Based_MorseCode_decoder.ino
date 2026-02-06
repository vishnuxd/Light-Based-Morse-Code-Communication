#include <TM1637Display.h>

// 7 segment pins
int clkPin = 6;
int dioPin = 7;

// LDR and buzzer
int ldrPin = A0;
int buzzerPin = 4;

TM1637Display display(clkPin, dioPin);

// Light threshold (change based on room light)
int lightThreshold = 900;

// Variables to detect signal time
unsigned long startTime = 0;
bool lightDetected = false;

// Store received morse for one letter
String receivedMorse = "";

// Store last 4 letters to show on display
char displayChars[4] = {' ', ' ', ' ', ' '};

// Morse table
String morseTable[26] = {
  ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---",
  "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", "...", "-",
  "..-", "...-", ".--", "-..-", "-.--", "--.."
};

char letters[26] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

void setup() {
  pinMode(buzzerPin, OUTPUT);
  Serial.begin(9600);

  display.setBrightness(0x0F);
  display.clear();

  Serial.println("LiFi Morse Receiver Started...");
}

void loop() {

  int ldrValue = analogRead(ldrPin);
  Serial.println(ldrValue);   // to adjust threshold

  // Light ON detected
  if (ldrValue > lightThreshold && lightDetected == false) {
    lightDetected = true;
    startTime = millis();
    tone(buzzerPin, 1000);
  }

  // Light OFF detected
  if (ldrValue <= lightThreshold && lightDetected == true) {
    lightDetected = false;
    unsigned long onTime = millis() - startTime;
    noTone(buzzerPin);

    if (onTime < 300) {
      receivedMorse = receivedMorse + ".";
      Serial.print(".");
    } else {
      receivedMorse = receivedMorse + "-";
      Serial.print("-");
    }
  }

  // Gap means one letter completed
  if (lightDetected == false && receivedMorse.length() > 0) {
    if (millis() - startTime > 1000) {

      char decoded = decodeMorseCode(receivedMorse);
      Serial.print(" -> ");
      Serial.println(decoded);

      if (decoded != '?') {
        updateDisplay(decoded);
      }

      receivedMorse = "";
    }
  }

  delay(50);
}

// Decode morse to alphabet
char decodeMorseCode(String morse) {
  for (int i = 0; i < 26; i++) {
    if (morse == morseTable[i]) {
      return letters[i];
    }
  }
  return '?';
}

// Update 7 segment display
void updateDisplay(char newChar) {

  displayChars[0] = displayChars[1];
  displayChars[1] = displayChars[2];
  displayChars[2] = displayChars[3];
  displayChars[3] = newChar;

  byte seg[4];
  seg[0] = getSegment(displayChars[0]);
  seg[1] = getSegment(displayChars[1]);
  seg[2] = getSegment(displayChars[2]);
  seg[3] = getSegment(displayChars[3]);

  display.setSegments(seg);
}

// Convert letter to 7 segment
byte getSegment(char ch) {
  if (ch == 'A') return 0x77;
  if (ch == 'B') return 0x7C;
  if (ch == 'C') return 0x39;
  if (ch == 'D') return 0x5E;
  if (ch == 'E') return 0x79;
  if (ch == 'F') return 0x71;
  if (ch == 'G') return 0x3D;
  if (ch == 'H') return 0x76;
  if (ch == 'I') return 0x06;
  if (ch == 'J') return 0x1F;
  if (ch == 'L') return 0x38;
  if (ch == 'O') return 0x3F;
  if (ch == 'S') return 0x6D;
  if (ch == 'U') return 0x3E;

  return 0x00;  // blank
}
