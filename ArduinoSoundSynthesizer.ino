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

bool songMode = false;
bool recording = false;
bool precording = false;
bool printSong = false;
bool saveEEPROM = true;

void setup() {

  Serial.begin(9600);
  Serial.println(F("Initializing PINs..."));

  pinMode(PIN, OUTPUT);
  
  for (int i = 0; i < 8; i++) {
    pinMode(pins[i], INPUT);
    digitalWrite(pins[i], HIGH); // turn on pullup resistors
    Serial.print(F("Initializing: "));
    Serial.println(i);
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
  if (EEPROM[0] == 255) {
    Serial.println(F("Detected empty or corrupt EEPROM. Loading default instead. "));

    songLength = 8;
    song[0] = 0;
    song[1] = 1;
    song[2] = 2;
    song[3] = 3;
    song[4] = 4;
    song[5] = 5;
    song[6] = 6;
    song[7] = 7;

    songMode = true;
  } else {
    EEPROM.get(0, song);
    for (int i = 0; i < 512; i++) {
      if (song[i] == 255) {
        songLength = i;
        break;
      }
      Serial.println(song[i]);      
    }
  }
  Serial.println(F("Loading complete"));
  Serial.println(F("Press any button to play"));
}

void loop() {  
  bool played = true;
  if (songMode && song[songIndex] == -100) {
    delay(100);
  }

  // Serial Reading 
  int data = 0;
  String cmd = "        ";
  if (Serial.available()) {
    data = Serial.read();
    if (data > 0 && data <= 90)
    {
      Serial.print(F("Received Command: "));
      Serial.println(data);
    }
    int i = 0;
    while(data > 90 && i < 8) {
      cmd[i] = data;
      data = Serial.read();
      i++;
    }
    if (cmd != "        ") {
      Serial.print(F("Received Command: "));
      Serial.println(cmd);
    }
  }

  //  Beginning playback
  if (digitalRead(PIN_SUB) == LOW && digitalRead(PIN_ADD) == LOW || cmd == "play    " || cmd == "playrec ") {
    songMode = true;
    interval = 0.1;
    songIndex = 0;
    Serial.println(F("Beginning song playback"));
  }

  // Stopping playback
  if (cmd == "stop   ") {
    songMode = false;
    songIndex = 0;
    Serial.println(F("Stopping song playback"));
  }

  if (cmd == "printoff") {
    printSong = false;
    Serial.println(F("Turning off note printing"));
  }

  if (cmd == "printon ") {
    printSong = true;
    Serial.println(F("Turning on note printing"));
  }

  if (cmd == "romoff  ") {
    printSong = false;
    Serial.println(F("Turning off note printing"));
  }

  if (cmd == "romon   ") {
    printSong = true;
    Serial.println(F("Turning on note printing"));
  }

  if (cmd == "printrom") {
    Serial.println(F("Printing EEPROM contents..."));
    for (int i = 0; i < EEPROM.length() && EEPROM[i] < 255; i++) {
      Serial.println(EEPROM[i]);
    }
    Serial.println(F("Print complete. "));
    if (EEPROM[0] == 255) {
      Serial.println(F("EEPROM was Empty. "));
    }
  }

  //All forms of song playback
  for (int i = 0; i < 8; i++) {
    if (digitalRead(pins[i]) == LOW || (songMode && i == song[songIndex]) || (data - 48) == i) {
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
      if (printSong) {
        Serial.println(i);
      }
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

  // EEPROM Clearing
  if (digitalRead(PIN_RECORD) == LOW && digitalRead(PIN_SUB) == LOW|| cmd == "clear   " || cmd == "clearrom") {
    delay(100);
    clearIndex++;
    if (clearIndex == 3) {
      Serial.println(F("Holding down both PIN_RECORD and PIN_SUB at the same time, Hold for 3 seconds to clear EEPROM"));
    }
    if (clearIndex == 31 || cmd == "clear   " || cmd == "clearrom") {
      ClearROM();
    }
  } else {
    clearIndex = 0;
  }

  // Song Recording
  if (digitalRead(PIN_RECORD) == LOW) {
    if (!precording) {
      precording = true;
      recording = !recording;
      if (recording) {
        songLength = 0;
        Serial.println(F("Beginning song record..."));
      } else {
        SaveROM();
      }
    }
  } else {
    precording = false;
  }
  // Record Commands
  if (cmd == "beginrec") {
    if (recording) {
      Serial.println(F("Restarting recording via command"));
    } else {
      Serial.println(F("Beginning recording via command"));
    }
    recording = true;
    songLength = 0;
  }
  if (cmd == "endrec  ") { 
    Serial.println(F("Ending record via command"));
    if (saveEEPROM) {
      SaveROM();
    } else {
      Serial.println(F("Saving to EEPROM was disabled. Use saverom to save. Use romon to enable automatic saving"));
    }

    recording = false;
    songIndex = 0;
  }

  if (cmd == "saverom ") {
    SaveROM();
  }

  // Song Mode Checks
  if (songMode) {
    SongChecks();
  }
}

void SaveROM() {
    Serial.println(F("Saving song to EEPROM..."));
    EEPROM.put(0, song);
    Serial.println(F("Save Sucessful."));
}

void ClearROM() {  
      Serial.println(F("Clearing EEPROM..."));

      for (int i = 0 ; i < EEPROM.length() ; i++) {
        EEPROM.write(i, 255);
      }
      clearIndex = 0;
      songMode = false;
      
      Serial.println(F("EEPROM sucessfully cleared"));
}

void SongChecks() {
  
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
