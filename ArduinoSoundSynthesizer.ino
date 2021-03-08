const int PIN = 2;
const int PIN_RECORD = 11;
const int PIN_SUB = 12;
const int PIN_ADD = 13;

float frequencies[48] = {130.82, 146.82, 164.81, 174.61, 196, 220, 246.94, 261.63, 293.66, 329.63, 349.23, 392, 440, 493.88, 523.25, 587.33, 659.26, 698.46, 784, 880, 987.77, 1046.5, 1174.66, 1318.51, 1396.91, 1568, 1760, 1975.53, 2093};
float frequencyMultiplier = 1;
float frequencyOffset = 0;
float interval = 0.1;
float dutyCycle = 0.25;

byte powersOfTwo[8] = {1, 2, 4, 8, 16, 32, 64, 128};

int transpose = 7;
int songLength = 9;
int song[] = {0, 1, 2, 3, 4, 5, 6, 7, -100};
int songIndex = 0;
int pins[8] = {3, 4, 5, 6, 7, 8, 9, 10};

bool songMode = true;
bool recording = false;

void setup() {
  pinMode(PIN, OUTPUT);

  for (int i = 0; i < 8; i++) {
    pinMode(pins[i], INPUT);
    digitalWrite(pins[i], HIGH); // turn on pullup resistors
  }
  Serial.begin(9600);
  //for (int i = 0; i < 48; i++) {
  //  frequencies[i] = 440 * powf(2, i / 12);
  //  Serial.println(frequencies[i]);
  //}

  Serial.end();
}

void loop() {  
  bool played = true;
  if (songMode && song[songIndex] == -100) {
    delay(400);
  }
  if (digitalRead(PIN_SUB) == LOW && digitalRead(PIN_ADD) == LOW) {
    songMode = true;
    interval = 0.4;
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
      played = false;
    }
  }

  if (digitalRead(PIN_RECORD)) {
    recording = true;
  }
  if (songMode) {
    delay(100);
    songIndex++;
    if (songIndex > songLength) {
      delay (500);
      songMode = false;
      interval = 0.1;
    }
  }
}

