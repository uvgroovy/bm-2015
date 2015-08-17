#include <Adafruit_NeoPixel.h>

#include <CapPin.h>
#include "Piezo.h"

#include <TimerOne.h>

// #define  DEBUG 1

#define CASSERT(X) typedef byte STATIC_ASSERT[X?1:-1]


#ifdef DEBUG
#define LEDPIN 13
#endif

#define PIEZO_PIN 9
#define NUM_PIXELS 8
#define LED_STRIP_PIN 5

CapPin cPin_C = CapPin(8);
CapPin cPin_Cs = CapPin(7);
CapPin cPin_D = CapPin(6);

CapPin cPin_Ds = CapPin(4);
CapPin cPin_E = CapPin(3);
CapPin cPin_F = CapPin(2);

CapPin cPin_Fs = CapPin(11);
CapPin cPin_G = CapPin(12);
CapPin cPin_Gs = CapPin(13);
CapPin cPin_A = CapPin(A3); // A0 wasn't working in my promini
CapPin cPin_As = CapPin(A1);
CapPin cPin_B = CapPin(A2);

CapPin* pins[] = { &cPin_C, &cPin_Cs, &cPin_D, &cPin_Ds, &cPin_E, &cPin_F, &cPin_Fs, &cPin_G, &cPin_Gs, &cPin_A, &cPin_As, &cPin_B};
byte names[] = {'c', 'C', 'd', 'D', 'e', 'f', 'F', 'g', 'G', 'a', 'A', 'b'};
// In Hz. x2 for a higher octave
int tones[]  = {261, 277, 294, 311, 329, 349, 370, 392, 415, 440, 466, 493};
int octaveExp = 0;

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))
const int NUM_PINS = ARRAY_SIZE(pins);

// static assert to make sure i didn't miss anything
CASSERT(NUM_PINS == ARRAY_SIZE(names) );
CASSERT(NUM_PINS == ARRAY_SIZE(tones) );


Piezo piezo(PIEZO_PIN);


#define THRESH 50
#define SAMPLES 1000


boolean isPressed(CapPin* pin) {
  long total = pin->readPin(SAMPLES);
  bool pressed =  total > THRESH;
#ifdef DEBUG
  if (pressed) {

    Serial.print("thresh is ");
    Serial.println(total);

  }
#endif

  return pressed;
}


Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, LED_STRIP_PIN);


//https://learn.adafruit.com/led-ampli-tie/the-code
// Input a value 0 to 255 to get a color value.
// The colors are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if (WheelPos < 85) {
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
  else if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  else {
    WheelPos -= 170;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

// Todo:
// 1. add mute buttons
// 2. and rainbow
// 3.

void setup() {
#ifdef DEBUG
  Serial.begin(9600);
#endif

  // initialize timer for the piezo class
  Timer1.initialize();
  // init led strip
  strip.begin();

#ifdef DEBUG
  pinMode(LEDPIN, OUTPUT);
#endif
  pinMode(PIEZO_PIN, OUTPUT);

  // read unconnected analog pin
  randomSeed(analogRead(6));

  for (int i = 0; i < NUM_PINS; ++i) {
#ifdef DEBUG
    Serial.print("Calibrating pin: ");
    Serial.println(i);
#endif
    pins[i]->calibratePin(SAMPLES);
  }
  delay(100);
  // not sure why but this seems to help
  for (int i = 0; i < NUM_PINS; ++i) {
#ifdef DEBUG
    Serial.print("reading pin for fun: ");
    Serial.println(i);
#endif
    pins[i]->readPin(SAMPLES);
  }

  strip.show(); // Initialize all pixels to 'off'

  // three beeps, all ok

#ifdef DEBUG
  digitalWrite(LEDPIN, HIGH);
#endif
  piezo.play(tones[4], 200);
  delay(100);
  piezo.play(tones[4], 200);
  delay(100);
  piezo.play(tones[4], 250);
  delay(150);
#ifdef DEBUG
  digitalWrite(LEDPIN, LOW);
#endif


#ifdef DEBUG
  Serial.println("Setup done");
#endif

}

typedef void (*Magicfunc)();

char magics[][5] = {
  {'c', 'C', 'C', 'C', 'C'},
  {'c', 'd', 'd', 'd', 'd'},
};
Magicfunc magicfuncs[] = { doMagic1,  doMagic4};

CASSERT(ARRAY_SIZE(magicfuncs) == ARRAY_SIZE(magics) );

void randomizeStrip() {

  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, random(0xFFFFFF));
    strip.show();
  }
}

void doMagic1() {
  byte melody[] =                "dafcdadcfdacdafcdaagp";
  byte times[sizeof(melody)] =   "221222222222221222228";
  byte octaves[sizeof(melody)] = "555555555555555555555";
  const int length = ARRAY_SIZE(melody);
  int tempo = 200;

  for (int i = 0; i < length; i++) {
    randomizeStrip();
    playStep(melody, times, octaves, i, tempo);
  }

}


void playStep(byte* melody, byte* times, byte* octaves, int i, int tempo) {

  byte timeLength = times[i] - '0';
  byte note = melody[i];
  int freq = 0;
  int octave = 0;
  for (int j = 0; j < sizeof(names) / sizeof(names[0]); j++) {
    if (names[j] == note) {
      freq = tones[j];
      octave = octaves[j] - '0' - 5;
    }
  }

  if (freq != 0 || note == 'p') {

    if (freq > 0) {
      if (octave >= 0) {
        freq <<= octave;
      } else {
        freq >>= (-octave);
      }
      piezo.start(freq);
    }
    delay(tempo * timeLength);
    piezo.stop();
  } else {
    // invalid note!
  }

}

void doMagic4() {
  // TODO animate
  // 8 pixel
}

int addNote(char note) {

  static char arr[5] = {'\0'};

  static uint8_t arrEndIndex = 0;

  arr[arrEndIndex] = note;
  // get begining index:

#ifdef DEBUG
  Serial.print("magic arr:");
  Serial.write(arr, ARRAY_SIZE(arr));
  Serial.println("");
#endif

  for (int i = 0; i < ARRAY_SIZE(magics); ++i) {
    uint8_t curIndex = (arrEndIndex + 1) % ARRAY_SIZE(arr);
    int j;

    for ( j = 0; j < ARRAY_SIZE(arr); ++j) {
      if (magics[i][j] != arr[curIndex]) {
        break;
      }
      curIndex = (curIndex + 1) % ARRAY_SIZE(arr);
    }
    if (j == ARRAY_SIZE(arr)) {
      // magic found, zero array
      memset(arr, 0, sizeof(arr));
      return i;
    }

  }

  arrEndIndex = (arrEndIndex + 1) % ARRAY_SIZE(arr);

  return -1;
}

int doLightShow(int special) {
  // play magic mode intro
  piezo.play(tones[4], 200);
  delay(100);
  piezo.play(tones[4], 200);
  delay(100);
  piezo.play(tones[4], 250);

  magicfuncs[special]();
  return 0;
}

void colorWipe(uint32_t c) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
  }
  strip.show();
}

inline int getnote(int i) {
  return (octaveExp < 0) ? tones[i] >> -octaveExp : tones[i] <<  octaveExp;
}


bool mute = false;
bool lightsoff = false;


const uint32_t red = strip.Color(255, 0, 0);
const uint32_t green = strip.Color(0, 255, 0);
const uint32_t blu = strip.Color(0, 0, 255);
const uint32_t yellow = strip.Color(255, 255, 0);

bool configCurrentPressed = false;
void doConfig() {

  if (piezo.isOn()) {
    piezo.stop();
  }
  // the strip goes red in config mode

  colorWipe(yellow);

  int i;
  for (i = 5; i < NUM_PINS; ++i) {
    if (isPressed(pins[i])) {
      break;
    }
  }

  bool anyButtonPressed = (i < NUM_PINS);
  if (anyButtonPressed && (!configCurrentPressed)) {
    // do something
    //   0    1    2    3    4    5    6    7    8    9   10   11
    // {'c', 'C', 'd', 'D', 'e', 'f', 'F', 'g', 'G', 'a', 'A', 'b'};
    switch (i) {
      case 1:
        mute = !mute;
        if (mute) {
          colorWipe(red);
        } else {
          colorWipe(green);
          piezo.play(getnote(0), 200);
        }
        delay(500);
        break;
      case 3:
        lightsoff = !lightsoff;
        if (lightsoff) {
          colorWipe(0);
        } else {
          colorWipe(yellow);
        }
        delay(500);
        break;
      case 5:
        octaveExp--;
        piezo.play(getnote(0), 200);
        break;
      case 7:
        octaveExp++;
        piezo.play(getnote(0), 200);
        break;


    }

  } else if ((!anyButtonPressed) && (configCurrentPressed)) {
    configCurrentPressed = false;
  }


}

bool currentPressed = false;
char currentNote = '\0';
int noteIndex = -1;
void loop() {
  /////////// get index of button
  int i;
  for (i = 0; i < NUM_PINS; ++i) {
    if (isPressed(pins[i])) {
      break;
    }
  }
  if (i == 0) {
    // check if we are in config mode
    if (isPressed(pins[2]) && isPressed(pins[4])) {
      doConfig();
      return;
    }

  }

  if (!mute) {
    playSounds(i);
  }

  if (!lightsoff) {
    animate();
  }
}

void playSounds(int i) {


  bool anyButtonPressed = (i < NUM_PINS);

  /// play a note, or stop playing one.
  if (anyButtonPressed) {

    // note changed
    if (currentPressed) {
      if (currentNote != names[i] ) {
        currentPressed = false;
        currentNote = '\0';
        piezo.stop();
      }
    }
    if (!currentPressed) {

#ifdef DEBUG
      Serial.print("Note index: ");
      Serial.println(i);
      digitalWrite(LEDPIN, HIGH);
#endif
      currentNote = names[i];
      currentPressed = true;
      noteIndex = i;
      int special = addNote(currentNote);

      if (special == -1) {


        piezo.start(getnote(i));
      } else {
        doLightShow(special);
      }
    }

  } else if ((!anyButtonPressed) && (currentPressed)) {
#ifdef DEBUG
    Serial.println("Note OFF");
    digitalWrite(LEDPIN, LOW);
#endif
    // key was released (no touch, and it was pressed before)
    currentPressed = false;
    noteIndex = -1;
    currentNote = '\0';
    piezo.stop();
  }
}


unsigned long lastDrawTime = 0;
unsigned long stepIndex = 0;
void animate() {
  /////////// calc frame
  unsigned long timePassed = millis();
  if ((timePassed - lastDrawTime) > 20) {
    // increase frame
    stepIndex++;
    stepIndex = stepIndex % 256;
    lastDrawTime = timePassed;
  }

  /////////// animate leds
  if (currentNote == '\0') {
    rainbow_step();
  }
  else {

#ifdef DEBUG
    boolean bit1 = noteIndex & 1;
    boolean bit2 = noteIndex & 2;
    boolean bit3 = noteIndex & 4;
    boolean bit4 = noteIndex & 8;

    for (uint16_t i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, yellow);
    }

    strip.setPixelColor(3, bit1 ? red : blu);
    strip.setPixelColor(4, bit2 ? red : blu);
    strip.setPixelColor(5, bit3 ? red : blu);
    strip.setPixelColor(6, bit4 ? red : blu);

#else
    uint32_t color = noteToColor(noteIndex);
    for (uint16_t i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, color);
    }
#endif

  }
  strip.show();
}

uint32_t noteToColor(int i) {

  return Wheel(map(i, 0, NUM_PINS - 1, 0, 255));
}

void rainbow_step() {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, Wheel((i + stepIndex) & 255));
  }
}


