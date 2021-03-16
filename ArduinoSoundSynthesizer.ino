#include <EEPROM.h>

const int PIN = 2;
const int PIN_RECORD = 11;
const int PIN_SUB = 12;
const int PIN_ADD = 13;

float frequencies[48] = {130.82, 146.82, 164.81, 174.61, 196, 220, 246.94, 261.63, 293.66, 329.63, 349.23, 392, 440, 493.88, 523.25, 587.33, 659.26, 698.46, 784, 880, 987.77, 1046.5, 1174.66, 1318.51, 1396.91, 1568, 1760, 1975.53, 2093};
float frequencyMultiplier = 1;
float frequencyOffset = 0;
float interval = 0.1;
float dutyCycle = 0.5;

byte transpose = 7;
int songLength = 512;
int songIndex = 0;
byte song[512] = {0, 1, 2, 3, 4, 5, 6, 7, -100};
byte pins[8] = {3, 4, 5, 6, 7, 8, 9, 10};
byte clearIndex = 0;

bool songMode = true;
bool recording = false;
bool precording = false;

void setup() {

  Serial.begin(9600);
  Serial.println(F("Initializing PINs..."));

  pinMode(PIN, OUTPUT);
  
  for (int i = 0; i < 8; i++) {
    pinMode(pins[i], INPUT);
    digitalWrite(pins[i], HIGH); // turn on pullup resistors
  }

  digitalWrite(PIN_RECORD, HIGH);
  digitalWrite(PIN_SUB, HIGH);
  digitalWrite(PIN_ADD, HIGH);
  
  Serial.println(F("PIN Initialization complete"));
  
  //for (int i = 0; i < 48; i++) {
  //  frequencies[i] = 440 * powf(2, i / 12);
  //  Serial.println(frequencies[i]);
  //}

  Serial.println(F("Loading song from EEPROM..."));
  EEPROM.get(0, song);
  for (int i = 0; i < 512; i++) {
    if (song[i] == 255) {
      songIndex = i;
      break;
    }
    Serial.println(song[i]);      
  }
  Serial.println(F("Loading complete"));
  Serial.println(F("Press any button to play"));
}

void loop() {  
  bool played = true;
  if (songMode && song[songIndex] == -100) {
    delay(100);
  }
  if (digitalRead(PIN_SUB) == LOW && digitalRead(PIN_ADD) == LOW) {
    songMode = true;
    interval = 0.1;
    songIndex = 0;
  }

  
  for (int i = 0; i < 8; i++) {
    if (digitalRead(pins[i]) == LOW || (songMode && i == song[songIndex])) {
      float t = 0;
      float a = frequencyMultiplier / (frequencies[i + transpose] + frequencyOffset);
      float d1 = 1000000 * a * dutyCycle;
      float d2 = 1000000 * a * (1 - dutyCycle);
      while (t < interval) {
        t += a;
        digitalWrite(PIN, true);
        delayMicroseconds (d1);
        digitalWrite(PIN, false);
        delayMicroseconds (d2);
      }
        Serial.println(i);
      if (recording) {
        song[songLength] = i;
        songLength++;
      }
      played = false;
    }
  }

  if (played && recording) {
    song[songLength] = -100;
    songLength++;
  }
  if (played) {
    delay(100);
  }

  if (digitalRead(PIN_RECORD) == LOW && digitalRead(PIN_SUB) == LOW) {
    delay(100);
    clearIndex++;
    if (clearIndex == 3) {
      Serial.println(F("Holding down both PIN_RECORD and PIN_SUB at the same time, Hold for 3 seconds to clear EEPROM"));
    }
    if (clearIndex == 31) {
      Serial.println(F("Clearing EEPROM..."));

      for (int i = 0 ; i < EEPROM.length() ; i++) {
        EEPROM.write(i, 255);
      }
      clearIndex = 0;

      songMode = false;
      
      Serial.println(F("EEPROM sucessfully cleared"));
    }
  } else {
    clearIndex = 0;
  }

  if (digitalRead(PIN_RECORD) == LOW) {
    if (!precording) {
      Serial.println(F("Beginning song record..."));
      precording = true;
      recording = !recording;
      if (recording) {
        songLength = 0;
      } else {
        Serial.println(F("Saving song to EEPROM..."));
        EEPROM.put(0, song);
        Serial.println(F("Save Sucessful."));
      }
    }
  } else {
    precording = false;
  }

  if (songMode) {
    if (interval == 0.05) {
      delay(50);
    }
    songIndex++;
    if (songIndex > songLength) {
      interval = 0.1;
      songMode = false;
      Serial.println(F("Playback Complete.."));
    }
    if (song[songIndex] == 255) {
      songMode = false;
    }
  }
}
