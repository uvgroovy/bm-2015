#include <Adafruit_NeoPixel.h>

#include <CapPin.h>
#include "Piezo.h"

#include <TimerOne.h>

#define  DEBUG 1

#define CASSERT(X) typedef byte STATIC_ASSERT[X?1:-1]

#define LEDPIN 13
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


void setup() {
#ifdef DEBUG
  Serial.begin(9600);
#endif

  // initialize timer for the piezo class
  Timer1.initialize();
  // init led strip
  strip.begin();

  pinMode(LEDPIN, OUTPUT);
  pinMode(PIEZO_PIN, OUTPUT);



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
  digitalWrite(LEDPIN, HIGH);
  piezo.play(329, 200);
  delay(100);
  piezo.play(329, 200);
  delay(100);
  piezo.play(329, 250);
  delay(150);
  digitalWrite(LEDPIN, LOW);


#ifdef DEBUG
  Serial.println("Setup done");
#endif

}

typedef void (*Magicfunc)();

char magics[][5] = {{'c','C','C','C','C'},};
Magicfunc magicfuncs[] = { doMagic1};

CASSERT(ARRAY_SIZE(magicfuncs) == ARRAY_SIZE(magics) );


void doMagic1() {
 
  theaterChaseRainbow(50);
}



//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
        for (int i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
        }
        strip.show();
       
        delay(wait);
       
        for (int i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, 0);        //turn every third pixel off
        }
    }
  }
}

byte melody[] =                "dafcdadcfdacdafcdaagp";
byte times[sizeof(melody)] =   "221222222222221222228";
byte octaves[sizeof(melody)] = "555555555555555555555";

int tempo = 70;

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
      return i;
    }

  }

  arrEndIndex = (arrEndIndex + 1) % ARRAY_SIZE(arr);

  return -1;
}

int doLightShow(int special) {
  // play magic mode intro
  piezo.play(329, 200);
  delay(100);
  piezo.play(329, 200);
  delay(100);
  piezo.play(329, 250);
  
  magicfuncs[special]();
  return 0;
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
#endif
      currentNote = names[i];
      currentPressed = true;
      noteIndex = i;
      digitalWrite(LEDPIN, HIGH);
      int special = addNote(currentNote);

      if (special == -1) {
        piezo.start(tones[i]);
      } else {
        doLightShow(special);
      }
    }

  } else if ((!anyButtonPressed) && (currentPressed)) {
#ifdef DEBUG
    Serial.println("Note OFF");
#endif
    // key was released (no touch, and it was pressed before)
    currentPressed = false;
    noteIndex = -1;
    currentNote = '\0';
    piezo.stop();
    digitalWrite(LEDPIN, LOW);
  }

  animate();
}


#ifdef DEBUG
const uint32_t red = strip.Color(255, 0, 0);
const uint32_t blu = strip.Color(0, 0, 255);
const uint32_t yellow = strip.Color(255, 255, 0);
#endif

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


